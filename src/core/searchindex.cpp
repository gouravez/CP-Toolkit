#include "searchindex.h"

#include <QRegularExpression>

void SearchIndex::build(const QList<Snippet> &snippets,
                        const QList<Solution> &solutions)
{
    m_index.clear();
    m_allResults.clear();

    for (const auto &s : snippets) {
        SearchResult r;
        r.id       = s.id;
        r.type     = SearchResult::Type::Snippet;
        r.title    = s.title;
        r.subtitle = s.category;
        m_allResults.append(r);

        for (const auto &token : tokenize(s.title))
            m_index[token].append(r);
        for (const auto &tag : s.tags)
            for (const auto &token : tokenize(tag))
                m_index[token].append(r);
    }

    for (const auto &s : solutions) {
        SearchResult r;
        r.id       = s.id;
        r.type     = SearchResult::Type::Solution;
        r.title    = s.title;
        r.subtitle = s.platform;
        m_allResults.append(r);

        for (const auto &token : tokenize(s.title))
            m_index[token].append(r);
        for (const auto &topic : s.topics)
            for (const auto &token : tokenize(topic))
                m_index[token].append(r);
    }
}

QList<SearchResult> SearchIndex::search(const QString &query) const
{
    if (query.trimmed().isEmpty())
        return {};

    QList<SearchResult> results;
    for (const auto &token : tokenize(query)) {
        if (m_index.contains(token)) {
            for (const auto &r : m_index[token]) {
                bool found = false;
                for (const auto &existing : results)
                    if (existing.id == r.id && existing.type == r.type) {
                        found = true;
                        break;
                    }
                if (!found)
                    results.append(r);
            }
        }
    }

    return results;
}

QList<QString> SearchIndex::tokenize(const QString &text) const
{
    return text.toLower().split(QRegularExpression("\\W+"),
                                Qt::SkipEmptyParts);
}