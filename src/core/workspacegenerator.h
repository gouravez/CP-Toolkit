#pragma once

#include <QList>
#include <QString>

class WorkspaceGenerator
{
public:
    WorkspaceGenerator() = default;

    void setBaseDirectory(const QString &dir);
    void setContestName(const QString &name);
    void setProblemCount(int count);

    bool create();

    QString workspacePath() const;
    QString problemFilePath(const QString &letter) const;
    QList<QString> problemLetters() const;
    QString errorString() const;

private:
    QString defaultTemplate() const;

    QString m_baseDir;
    QString m_contestName;
    int     m_problemCount = 6;

    QString        m_workspacePath;
    QList<QString> m_letters;
    QString        m_errorString;
};