#pragma once

#include "../models.h"

#include <QString>
#include <QList>

class TemplateGenerator
{
public:
    struct Result {
        bool    success = false;
        QString code;
        QString message;
    };

    Result generate(const QList<Snippet> &snippets);

private:
    QList<QString> extractIncludes(const QString &code);
    QString        removeIncludes(const QString &code);
};