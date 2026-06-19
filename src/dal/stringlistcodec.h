#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <QStringList>

namespace StringListCodec
{
    inline QString encode(const QStringList &items)
    {
        QJsonArray arr;
        for (const QString &item : items)
            arr.append(item);
        return QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
    }

    inline QStringList decode(const QString &stored)
    {
        if (stored.isEmpty())
            return {};

        const QJsonDocument doc = QJsonDocument::fromJson(stored.toUtf8());
        if (doc.isArray()) {
            QStringList result;
            for (const QJsonValue &v : doc.array())
                result.append(v.toString());
            return result;
        }

        return stored.split(QLatin1Char(','), Qt::SkipEmptyParts);
    }
}