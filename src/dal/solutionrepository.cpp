#include "solutionrepository.h"

#include <QDateTime>

SolutionRepository::SolutionRepository(IDatabase *db)
    : m_db(db)
{}

bool SolutionRepository::initialize()
{
    return m_db->execute(
        "CREATE TABLE IF NOT EXISTS solutions ("
        "  id          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  title       TEXT NOT NULL,"
        "  code        TEXT NOT NULL DEFAULT '',"
        "  platform    TEXT NOT NULL DEFAULT '',"
        "  difficulty  INTEGER NOT NULL DEFAULT 0,"
        "  problem_url TEXT NOT NULL DEFAULT '',"
        "  topics      TEXT NOT NULL DEFAULT '',"
        "  created_at  TEXT NOT NULL,"
        "  updated_at  TEXT NOT NULL"
        ");"
    );
}

int SolutionRepository::create(const Solution &solution)
{
    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);

    bool ok = m_db->execute(
        "INSERT INTO solutions(title, code, platform, difficulty, problem_url, topics, created_at, updated_at) "
        "VALUES(:title, :code, :platform, :difficulty, :problem_url, :topics, :created_at, :updated_at);",
        {
            {":title",       solution.title},
            {":code",        solution.code},
            {":platform",    solution.platform},
            {":difficulty",  solution.difficulty},
            {":problem_url", solution.problemUrl},
            {":topics",      solution.topics.join(",")},
            {":created_at",  now},
            {":updated_at",  now}
        }
    );

    return ok ? static_cast<int>(m_db->lastInsertId()) : -1;
}

QList<Solution> SolutionRepository::getAll()
{
    auto rows = m_db->query("SELECT * FROM solutions ORDER BY title ASC;");
    QList<Solution> result;
    for (const auto &row : rows)
        result.append(rowToSolution(row));
    return result;
}

QList<Solution> SolutionRepository::getByPlatform(const QString &platform)
{
    auto rows = m_db->query(
        "SELECT * FROM solutions WHERE platform = :platform ORDER BY title ASC;",
        {{":platform", platform}}
    );
    QList<Solution> result;
    for (const auto &row : rows)
        result.append(rowToSolution(row));
    return result;
}

QList<Solution> SolutionRepository::getByDifficulty(int min, int max)
{
    auto rows = m_db->query(
        "SELECT * FROM solutions WHERE difficulty BETWEEN :min AND :max ORDER BY difficulty ASC;",
        {{":min", min}, {":max", max}}
    );
    QList<Solution> result;
    for (const auto &row : rows)
        result.append(rowToSolution(row));
    return result;
}

bool SolutionRepository::update(const Solution &solution)
{
    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    return m_db->execute(
        "UPDATE solutions SET title=:title, code=:code, platform=:platform, "
        "difficulty=:difficulty, problem_url=:problem_url, topics=:topics, "
        "updated_at=:updated_at WHERE id=:id;",
        {
            {":title",       solution.title},
            {":code",        solution.code},
            {":platform",    solution.platform},
            {":difficulty",  solution.difficulty},
            {":problem_url", solution.problemUrl},
            {":topics",      solution.topics.join(",")},
            {":updated_at",  now},
            {":id",          solution.id}
        }
    );
}

bool SolutionRepository::remove(int id)
{
    return m_db->execute(
        "DELETE FROM solutions WHERE id=:id;",
        {{":id", id}}
    );
}

Solution SolutionRepository::rowToSolution(const QVariantMap &row)
{
    Solution s;
    s.id         = row["id"].toInt();
    s.title      = row["title"].toString();
    s.code       = row["code"].toString();
    s.platform   = row["platform"].toString();
    s.difficulty = row["difficulty"].toInt();
    s.problemUrl = row["problem_url"].toString();
    s.createdAt  = row["created_at"].toString();
    s.updatedAt  = row["updated_at"].toString();

    QString topics = row["topics"].toString();
    if (!topics.isEmpty())
        s.topics = topics.split(",");

    return s;
}