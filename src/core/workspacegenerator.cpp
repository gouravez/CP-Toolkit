#include "workspacegenerator.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

WorkspaceGenerator::Result WorkspaceGenerator::generate(const Config &config)
{
    Result result;

    if (config.contestName.trimmed().isEmpty()) {
        result.message = "Contest name cannot be empty.";
        return result;
    }

    if (config.problems.isEmpty()) {
        result.message = "No problems specified.";
        return result;
    }

    QDir base(config.basePath);
    QString folderName = config.contestName.trimmed();
    folderName.replace(' ', '_');

    if (!base.mkpath(folderName)) {
        result.message = "Failed to create contest directory.";
        return result;
    }

    QString contestPath = base.filePath(folderName);

    for (const QString &problem : config.problems) {
        QString filePath = contestPath + "/" + problem + ".cpp";
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            continue;

        QTextStream out(&file);
        if (!config.boilerplate.isEmpty())
            out << config.boilerplate;
        else
            out << "#include <bits/stdc++.h>\nusing namespace std;\n\nint main() {\n    \n    return 0;\n}\n";
    }

    result.success       = true;
    result.message       = "Workspace created successfully.";
    result.workspacePath = contestPath;
    return result;
}