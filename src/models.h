#pragma once

#include <QString>
#include <QList>

struct Snippet
{
    int     id         = -1;
    QString title;
    QString code;
    QString category;
    QList<QString> tags;
    QString createdAt;
    QString updatedAt;
};

struct Solution
{
    int     id         = -1;
    QString title;
    QString code;
    QString platform;
    int     difficulty = 0;
    QString problemUrl;
    QList<QString> topics;
    QString createdAt;
    QString updatedAt;
};

struct StressTestRun
{
    int     id            = -1;
    QString generatorPath;
    QString brutePath;
    QString optimizedPath;
};

struct SearchResult
{
    enum class Type { Snippet, Solution };

    int     id;
    Type    type;
    QString title;
    QString subtitle;
};