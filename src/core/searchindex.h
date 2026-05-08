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
    void upsert(const Snippet &snippet);
    void upsert(const Solution &solution);
    void removeSnippet(int id);
    void removeSolution(int id);

private:
    QList<QString> tokenize(const QString &text) const;

    QHash<QString, QList<SearchResult>> m_index;
    QList<SearchResult> m_allResults;
};