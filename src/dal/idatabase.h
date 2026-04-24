#pragma once

#include <QString>
#include <QList>
#include <QVariantMap>

class IDatabase
{
public:
    virtual ~IDatabase() = default;

    virtual bool open()  = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    virtual bool execute(const QString &sql,
                         const QVariantMap &bindings = {}) = 0;

    virtual QList<QVariantMap> query(const QString &sql,
                                     const QVariantMap &bindings = {}) = 0;
};