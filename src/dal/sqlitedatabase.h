#pragma once

#include "idatabase.h"

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>

class SqliteDatabase : public IDatabase
{
public:
    explicit SqliteDatabase(const QString &path);
    ~SqliteDatabase() override;

    bool open()  override;
    void close() override;
    bool isOpen() const override;

    bool execute(const QString &sql,
                 const QVariantMap &bindings = {}) override;

    QList<QVariantMap> query(const QString &sql,
                              const QVariantMap &bindings = {}) override;

    qint64 lastInsertId() const override;

    bool beginTransaction()    override;
    bool commitTransaction()   override;
    bool rollbackTransaction() override;

private:
    QString       m_path;
    QSqlDatabase  m_db;
    qint64        m_lastInsertId = -1;
};