#pragma once

#include "idatabase.h"
#include "../models.h"

#include <QList>
#include <QString>

class SnippetRepository
{
public:
    explicit SnippetRepository(IDatabase *db);

    bool        initialize();
    int         create(const Snippet &snippet);
    Snippet     getById(int id);
    QList<Snippet> getAll();
    QList<Snippet> getByCategory(const QString &category);
    QList<QString> getCategories();
    bool        update(const Snippet &snippet);
    bool        remove(int id);

private:
    Snippet     rowToSnippet(const QVariantMap &row);
    IDatabase  *m_db;
};