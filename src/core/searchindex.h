#pragma once

#include "../models.h"

#include <QHash>
#include <QList>
#include <QString>

class SearchIndex
{
public:
    SearchIndex() = default;

    void build(const QList<Snippet> &snippets,
               const QList<Solution> &solutions);

    QList<SearchResult> search(const QString &query) const;

private:
    QList<QString> tokenize(const QString &text) const;

    QHash<QString, QList<SearchResult>> m_index;
    QList<SearchResult> m_allResults;
};