#include "Cppsyntaxhighlighter.h"

#include <QColor>
#include <QFont>

CppSyntaxHighlighter::CppSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    setupRules();
}

void CppSyntaxHighlighter::setupRules()
{
    m_keywordFormat.setForeground(QColor("#c586c0"));
    m_keywordFormat.setFontWeight(QFont::Bold);

    const QStringList keywords = {
        "auto", "bool", "break", "case", "catch", "class", "const",
        "constexpr", "continue", "default", "delete", "do", "double",
        "else", "enum", "explicit", "false", "float", "for", "friend",
        "if", "inline", "int", "long", "mutable", "namespace", "new",
        "noexcept", "nullptr", "operator", "override", "private",
        "protected", "public", "return", "short", "signed", "sizeof",
        "static", "struct", "switch", "template", "this", "throw",
        "true", "try", "typedef", "typename", "union", "unsigned",
        "using", "virtual", "void", "volatile", "while"
    };

    for (const QString &kw : keywords) {
        HighlightRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\\b%1\\b").arg(kw));
        rule.format  = m_keywordFormat;
        m_rules.append(rule);
    }

    m_typeFormat.setForeground(QColor("#4ec9b0"));
    const QStringList types = {
        "string", "vector", "map", "set", "pair", "queue", "stack",
        "deque", "unordered_map", "unordered_set", "priority_queue",
        "array", "int32_t", "int64_t", "uint32_t", "uint64_t", "size_t"
    };
    for (const QString &t : types) {
        HighlightRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\\b%1\\b").arg(t));
        rule.format  = m_typeFormat;
        m_rules.append(rule);
    }

    m_numberFormat.setForeground(QColor("#b5cea8"));
    {
        HighlightRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9]+\\.?[0-9]*\\b"));
        rule.format  = m_numberFormat;
        m_rules.append(rule);
    }

    m_stringFormat.setForeground(QColor("#ce9178"));
    {
        HighlightRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("\".*?\"|'.*?'"));
        rule.format  = m_stringFormat;
        m_rules.append(rule);
    }

    m_commentFormat.setForeground(QColor("#6a9955"));
    m_commentFormat.setFontItalic(true);
    {
        HighlightRule rule;
        rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
        rule.format  = m_commentFormat;
        m_rules.append(rule);
    }
}

void CppSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightRule &rule : m_rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            const QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}