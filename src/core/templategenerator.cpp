#include "templategenerator.h"

#include <QRegularExpression>
#include <QStringList>

TemplateGenerator::Result TemplateGenerator::generate(const QList<Snippet> &snippets)
{
    Result result;

    if (snippets.isEmpty()) {
        result.message = "No snippets selected.";
        return result;
    }

    QList<QString> seenIncludes;
    QStringList    bodies;

    for (const auto &snippet : snippets) {
        for (const auto &inc : extractIncludes(snippet.code)) {
            if (!seenIncludes.contains(inc))
                seenIncludes.append(inc);
        }
        QString body = removeIncludes(snippet.code).trimmed();
        if (!body.isEmpty())
            bodies.append("// --- " + snippet.title + " ---\n" + body);
    }

    QString final;
    for (const auto &inc : seenIncludes)
        final += inc + "\n";

    if (!seenIncludes.isEmpty())
        final += "\n";

    final += "using namespace std;\n\n";
    final += bodies.join("\n\n");

    result.success = true;
    result.code    = final;
    result.message = "Template generated successfully.";
    return result;
}

QList<QString> TemplateGenerator::extractIncludes(const QString &code)
{
    QList<QString> includes;
    QRegularExpression re(R"(^\s*#include\s*[<"][^>"]+[>"])");
    re.setPatternOptions(QRegularExpression::MultilineOption);

    auto it = re.globalMatch(code);
    while (it.hasNext()) {
        QString inc = it.next().captured().trimmed();
        if (!includes.contains(inc))
            includes.append(inc);
    }
    return includes;
}

QString TemplateGenerator::removeIncludes(const QString &code)
{
    QRegularExpression re(R"(^\s*#include\s*[<"][^>"]+[>"]\s*\n?)");
    re.setPatternOptions(QRegularExpression::MultilineOption);
    QString result = code;
    result.remove(re);
    return result;
}