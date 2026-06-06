#pragma once

#include "idatabase.h"
#include "../models.h"

#include <QList>
#include <QString>

class SolutionRepository
{
public:
    explicit SolutionRepository(IDatabase *db);

    bool           initialize();
    int            create(const Solution &solution);
    Solution       getById(int id);
    QList<Solution> getAll();
    QList<Solution> getByPlatform(const QString &platform);
    QList<Solution> getByDifficulty(int min, int max);
    QList<QString> getPlatforms();
    bool           update(const Solution &solution);
    bool           remove(int id);

private:
    Solution    rowToSolution(const QVariantMap &row);
    IDatabase  *m_db;
};