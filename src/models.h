#pragma once

#include <QString>
#include <QList>

struct Snippet
{
    int     id       = -1;
    QString title;
    QString code;
    QString category;
};

struct Solution
{
    int     id         = -1;
    QString title;
    QString code;
    QString platform;
    int     difficulty = 0;
};

struct SearchResult
{
    enum class Type { Snippet, Solution };

    int     id;
    Type    type;
    QString title;
};