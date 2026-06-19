#pragma once

#include "../models.h"

#include <QHash>
#include <QList>
#include <QRegularExpression>
#include <QString>

class SearchIndex
{
public:
    SearchIndex() = default;

    void build(const QList<Snippet> &snippets,
               const QList<Solution> &solutions);

    void upsert(const Snippet &snippet);
    void upsert(const Solution &solution);

    void removeSnippet(int id);
    void removeSolution(int id);

    QList<SearchResult> search(const QString &query) const;

private:
    QList<QString> tokenize(const QString &text) const;

    QHash<QString, QList<SearchResult>> m_index;

    QList<SearchResult> m_allResults;
};