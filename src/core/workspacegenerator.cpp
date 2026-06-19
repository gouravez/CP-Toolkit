#include "workspacegenerator.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

void WorkspaceGenerator::setBaseDirectory(const QString &dir)
{
    m_baseDir = dir;
}

void WorkspaceGenerator::setContestName(const QString &name)
{
    m_contestName = name;
}

void WorkspaceGenerator::setProblemCount(int count)
{
    m_problemCount = qBound(1, count, 26);
}

void WorkspaceGenerator::setCustomTemplate(const QString &content)
{
    m_customTemplate    = content;
    m_hasCustomTemplate = !content.trimmed().isEmpty();
}

void WorkspaceGenerator::clearCustomTemplate()
{
    m_customTemplate.clear();
    m_hasCustomTemplate = false;
}

bool WorkspaceGenerator::hasCustomTemplate() const
{
    return m_hasCustomTemplate;
}

QString WorkspaceGenerator::workspacePath() const
{
    return m_workspacePath;
}

QString WorkspaceGenerator::problemFilePath(const QString &letter) const
{
    if (m_workspacePath.isEmpty())
        return {};
    return QDir(m_workspacePath).filePath(letter + QStringLiteral(".cpp"));
}

QList<QString> WorkspaceGenerator::problemLetters() const
{
    return m_letters;
}

QString WorkspaceGenerator::errorString() const
{
    return m_errorString;
}

bool WorkspaceGenerator::create()
{
    m_letters.clear();
    m_workspacePath.clear();
    m_errorString.clear();

    if (m_baseDir.isEmpty()) {
        m_errorString = QStringLiteral("Base directory is not set.");
        return false;
    }

    if (m_contestName.trimmed().isEmpty()) {
        m_errorString = QStringLiteral("Contest name must not be empty.");
        return false;
    }

    const QString folderName = m_contestName.trimmed();
    QDir base(m_baseDir);

    if (!base.exists()) {
        m_errorString = QStringLiteral("Base directory does not exist: %1").arg(m_baseDir);
        return false;
    }

    if (!base.mkpath(folderName)) {
        m_errorString = QStringLiteral("Failed to create workspace directory.");
        return false;
    }

    m_workspacePath = base.absoluteFilePath(folderName);
    QDir workspace(m_workspacePath);

    const QString tmpl = activeTemplate();

    for (int i = 0; i < m_problemCount; ++i) {
        const QString letter = QString(QChar(QLatin1Char(static_cast<char>('A' + i))));
        const QString path   = workspace.filePath(letter + QStringLiteral(".cpp"));

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            m_errorString = QStringLiteral("Could not write file: %1").arg(path);
            return false;
        }

        QTextStream ts(&file);
        ts << tmpl;

        m_letters.append(letter);
    }

    return true;
}

QString WorkspaceGenerator::activeTemplate() const
{
    return m_hasCustomTemplate ? m_customTemplate : defaultTemplate();
}

QString WorkspaceGenerator::defaultTemplate() const
{
    return QStringLiteral(
        "#include <bits/stdc++.h>\n"
        "using namespace std;\n\n"
        "int main()\n"
        "{\n"
        "    ios::sync_with_stdio(false);\n"
        "    cin.tie(nullptr);\n\n"
        "    // TODO\n\n"
        "    return 0;\n"
        "}\n");
}