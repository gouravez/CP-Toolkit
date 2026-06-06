#include "snippetrepository.h"

#include <QDateTime>

SnippetRepository::SnippetRepository(IDatabase *db)
    : m_db(db)
{}

bool SnippetRepository::initialize()
{
    return m_db->execute(
        "CREATE TABLE IF NOT EXISTS snippets ("
        "  id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  title      TEXT NOT NULL,"
        "  code       TEXT NOT NULL DEFAULT '',"
        "  category   TEXT NOT NULL DEFAULT '',"
        "  tags       TEXT NOT NULL DEFAULT '',"
        "  created_at TEXT NOT NULL,"
        "  updated_at TEXT NOT NULL"
        ");"
    );
}

int SnippetRepository::create(const Snippet &snippet)
{
    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);

    bool ok = m_db->execute(
        "INSERT INTO snippets(title, code, category, tags, created_at, updated_at) "
        "VALUES(:title, :code, :category, :tags, :created_at, :updated_at);",
        {
            {":title",      snippet.title},
            {":code",       snippet.code},
            {":category",   snippet.category},
            {":tags",       snippet.tags.join(",")},
            {":created_at", now},
            {":updated_at", now}
        }
    );

    return ok ? static_cast<int>(m_db->lastInsertId()) : -1;
}

Snippet SnippetRepository::getById(int id)
{
    auto rows = m_db->query(
        "SELECT * FROM snippets WHERE id = :id;",
        {{":id", id}}
    );
    if (rows.isEmpty())
        return {};
    return rowToSnippet(rows.first());
}

QList<Snippet> SnippetRepository::getAll()
{
    auto rows = m_db->query("SELECT * FROM snippets ORDER BY title ASC;");
    QList<Snippet> result;
    for (const auto &row : rows)
        result.append(rowToSnippet(row));
    return result;
}

QList<Snippet> SnippetRepository::getByCategory(const QString &category)
{
    auto rows = m_db->query(
        "SELECT * FROM snippets WHERE category = :category ORDER BY title ASC;",
        {{":category", category}}
    );
    QList<Snippet> result;
    for (const auto &row : rows)
        result.append(rowToSnippet(row));
    return result;
}

QList<QString> SnippetRepository::getCategories()
{
    auto rows = m_db->query(
        "SELECT DISTINCT category FROM snippets WHERE category != '' ORDER BY category ASC;"
    );
    QList<QString> result;
    for (const auto &row : rows)
        result.append(row["category"].toString());
    return result;
}

bool SnippetRepository::update(const Snippet &snippet)
{
    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    return m_db->execute(
        "UPDATE snippets SET title=:title, code=:code, category=:category, "
        "tags=:tags, updated_at=:updated_at WHERE id=:id;",
        {
            {":title",      snippet.title},
            {":code",       snippet.code},
            {":category",   snippet.category},
            {":tags",       snippet.tags.join(",")},
            {":updated_at", now},
            {":id",         snippet.id}
        }
    );
}

bool SnippetRepository::remove(int id)
{
    return m_db->execute(
        "DELETE FROM snippets WHERE id=:id;",
        {{":id", id}}
    );
}

Snippet SnippetRepository::rowToSnippet(const QVariantMap &row)
{
    Snippet s;
    s.id        = row["id"].toInt();
    s.title     = row["title"].toString();
    s.code      = row["code"].toString();
    s.category  = row["category"].toString();
    s.createdAt = row["created_at"].toString();
    s.updatedAt = row["updated_at"].toString();

    QString tags = row["tags"].toString();
    if (!tags.isEmpty())
        s.tags = tags.split(",");

    return s;
}