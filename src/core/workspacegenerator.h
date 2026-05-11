#pragma once

#include <QString>
#include <QList>

class WorkspaceGenerator
{
public:
    struct Config {
        QString contestName;
        QString basePath;
        QList<QString> problems;
        QString boilerplate;
    };

    struct Result {
        bool    success = false;
        QString message;
        QString workspacePath;
    };

    Result generate(const Config &config);
};