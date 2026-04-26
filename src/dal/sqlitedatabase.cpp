#include "sqlitedatabase.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SqliteDatabase::SqliteDatabase(const QString &path)
    : m_path(path)
{}

SqliteDatabase::~SqliteDatabase()
{
    close();
}

bool SqliteDatabase::open()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_path);

    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    execute("PRAGMA foreign_keys = ON;");
    return true;
}

void SqliteDatabase::close()
{
    if (m_db.isOpen())
        m_db.close();
}

bool SqliteDatabase::isOpen() const
{
    return m_db.isOpen();
}

bool SqliteDatabase::execute(const QString &sql, const QVariantMap &bindings)
{
    QSqlQuery query(m_db);
    query.prepare(sql);

    for (auto it = bindings.constBegin(); it != bindings.constEnd(); ++it)
        query.bindValue(it.key(), it.value());

    if (!query.exec()) {
        qWarning() << "Query failed:" << query.lastError().text();
        return false;
    }

    m_lastInsertId = query.lastInsertId().toLongLong();
    return true;
}

QList<QVariantMap> SqliteDatabase::query(const QString &sql, const QVariantMap &bindings)
{
    QSqlQuery query(m_db);
    query.prepare(sql);

    for (auto it = bindings.constBegin(); it != bindings.constEnd(); ++it)
        query.bindValue(it.key(), it.value());

    QList<QVariantMap> results;

    if (!query.exec()) {
        qWarning() << "Query failed:" << query.lastError().text();
        return results;
    }

    while (query.next()) {
        QVariantMap row;
        for (int i = 0; i < query.record().count(); ++i)
            row[query.record().fieldName(i)] = query.value(i);
        results.append(row);
    }

    return results;
}

qint64 SqliteDatabase::lastInsertId() const
{
    return m_lastInsertId;
}

bool SqliteDatabase::beginTransaction()
{
    return m_db.transaction();
}

bool SqliteDatabase::commitTransaction()
{
    return m_db.commit();
}

bool SqliteDatabase::rollbackTransaction()
{
    return m_db.rollback();
}