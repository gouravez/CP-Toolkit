#include "searchindex.h"

#include <QStringList>
#include <algorithm>

// ---------------------------------------------------------------------------
void SearchIndex::build(const QList<Snippet> &snippets,
                        const QList<Solution> &solutions)
{
    m_index.clear();
    m_allResults.clear();

    for (const Snippet &s : snippets)
        upsert(s);

    for (const Solution &s : solutions)
        upsert(s);
}

// ---------------------------------------------------------------------------
void SearchIndex::upsert(const Snippet &snippet)
{
    removeSnippet(snippet.id);

    SearchResult result;
    result.id       = snippet.id;
    result.type     = SearchResult::Type::Snippet;
    result.title    = snippet.title;
    result.subtitle = snippet.category;

    m_allResults.append(result);

    for (const QString &token : tokenize(snippet.title))
        m_index[token].append(result);

    for (const QString &token : tokenize(snippet.category))
        m_index[token].append(result);

    for (const QString &tag : snippet.tags)
        for (const QString &token : tokenize(tag))
            m_index[token].append(result);
}

// ---------------------------------------------------------------------------
void SearchIndex::upsert(const Solution &solution)
{
    removeSolution(solution.id);

    SearchResult result;
    result.id       = solution.id;
    result.type     = SearchResult::Type::Solution;
    result.title    = solution.title;
    result.subtitle = solution.platform;

    m_allResults.append(result);

    for (const QString &token : tokenize(solution.title))
        m_index[token].append(result);

    for (const QString &token : tokenize(solution.platform))
        m_index[token].append(result);

    for (const QString &topic : solution.topics)
        for (const QString &token : tokenize(topic))
            m_index[token].append(result);
}

// ---------------------------------------------------------------------------
void SearchIndex::removeSnippet(int id)
{
    m_allResults.removeIf([id](const SearchResult &r) {
        return r.type == SearchResult::Type::Snippet && r.id == id;
    });

    for (auto &list : m_index) {
        list.removeIf([id](const SearchResult &r) {
            return r.type == SearchResult::Type::Snippet && r.id == id;
        });
    }
}

// ---------------------------------------------------------------------------
void SearchIndex::removeSolution(int id)
{
    m_allResults.removeIf([id](const SearchResult &r) {
        return r.type == SearchResult::Type::Solution && r.id == id;
    });

    for (auto &list : m_index) {
        list.removeIf([id](const SearchResult &r) {
            return r.type == SearchResult::Type::Solution && r.id == id;
        });
    }
}

// ---------------------------------------------------------------------------
QList<SearchResult> SearchIndex::search(const QString &query) const
{
    if (query.trimmed().isEmpty())
        return m_allResults;

    const QList<QString> tokens = tokenize(query);
    if (tokens.isEmpty())
        return {};

    QHash<int, int> scoreMap;        
    QHash<int, SearchResult> byKey;  

    auto makeKey = [](const SearchResult &r) {
        return (r.type == SearchResult::Type::Snippet ? 0 : 100000) + r.id;
    };

    for (const QString &token : tokens) {
        if (m_index.contains(token)) {
            for (const SearchResult &r : m_index[token]) {
                const int key = makeKey(r);
                byKey[key] = r;

                const bool isTitleMatch =
                    r.title.toLower().contains(token);

                scoreMap[key] += isTitleMatch ? 2 : 1;
            }
        }

        for (auto it = m_index.constBegin(); it != m_index.constEnd(); ++it) {
            if (it.key().startsWith(token) && it.key() != token) {
                for (const SearchResult &r : it.value()) {
                    const int key = makeKey(r);
                    byKey[key] = r;
                    scoreMap[key] += 1;
                }
            }
        }
    }

    QList<SearchResult> results = byKey.values();
    std::sort(results.begin(), results.end(),
              [&scoreMap, &makeKey](const SearchResult &a, const SearchResult &b) {
                  return scoreMap[makeKey(a)] > scoreMap[makeKey(b)];
              });

    return results;
}

QList<QString> SearchIndex::tokenize(const QString &text) const
{
    const QStringList parts = text.toLower().split(
        QRegularExpression("[^a-z0-9]+"), Qt::SkipEmptyParts);

    QList<QString> tokens;
    for (const QString &p : parts)
        if (!p.isEmpty())
            tokens.append(p);

    return tokens;
}