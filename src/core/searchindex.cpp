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

    QString lower = query.toLower().trimmed();
    QList<SearchResult> exact;
    QList<SearchResult> rest;

    for (const auto &token : tokenize(query)) {
        if (!m_index.contains(token))
            continue;
        for (const auto &r : m_index[token]) {
            bool already = false;
            for (const auto &e : exact)
                if (e.id == r.id && e.type == r.type) { already = true; break; }
            for (const auto &e : rest)
                if (e.id == r.id && e.type == r.type) { already = true; break; }
            if (already) continue;

            if (r.title.toLower() == lower)
                exact.append(r);
            else
                rest.append(r);
        }
    }

    return exact + rest;
}

QList<QString> SearchIndex::tokenize(const QString &text) const
{
    return text.toLower().split(QRegularExpression("\\W+"),
                                Qt::SkipEmptyParts);
}

void SearchIndex::upsert(const Snippet &snippet)
{
    removeSnippet(snippet.id);

    SearchResult r;
    r.id       = snippet.id;
    r.type     = SearchResult::Type::Snippet;
    r.title    = snippet.title;
    r.subtitle = snippet.category;
    m_allResults.append(r);

    for (const auto &token : tokenize(snippet.title))
        m_index[token].append(r);
    for (const auto &tag : snippet.tags)
        for (const auto &token : tokenize(tag))
            m_index[token].append(r);
}

void SearchIndex::upsert(const Solution &solution)
{
    removeSolution(solution.id);

    SearchResult r;
    r.id       = solution.id;
    r.type     = SearchResult::Type::Solution;
    r.title    = solution.title;
    r.subtitle = solution.platform;
    m_allResults.append(r);

    for (const auto &token : tokenize(solution.title))
        m_index[token].append(r);
    for (const auto &topic : solution.topics)
        for (const auto &token : tokenize(topic))
            m_index[token].append(r);
}

void SearchIndex::removeSnippet(int id)
{
    m_allResults.removeIf([id](const SearchResult &r){
        return r.type == SearchResult::Type::Snippet && r.id == id;
    });
    for (auto &list : m_index)
        list.removeIf([id](const SearchResult &r){
            return r.type == SearchResult::Type::Snippet && r.id == id;
        });
}

void SearchIndex::removeSolution(int id)
{
    m_allResults.removeIf([id](const SearchResult &r){
        return r.type == SearchResult::Type::Solution && r.id == id;
    });
    for (auto &list : m_index)
        list.removeIf([id](const SearchResult &r){
            return r.type == SearchResult::Type::Solution && r.id == id;
        });
}