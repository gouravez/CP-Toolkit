#pragma once

#include "../models.h"

#include <QList>
#include <QString>

class TemplateGenerator
{
public:
    TemplateGenerator() = default;

    void setSnippets(const QList<Snippet> &snippets);
    QString generate() const;
    bool exportToFile(const QString &filePath) const;

private:
    QList<QString> extractIncludes(const QString &code) const;
    QString stripIncludes(const QString &code) const;
    QString toNamespaceId(const QString &title) const;

    QList<Snippet> m_snippets;
};