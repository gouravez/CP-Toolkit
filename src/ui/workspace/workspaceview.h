#pragma once

#include "../../models.h"

#include <QWidget>
#include <QList>

class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class CppSyntaxHighlighter;

class WorkspaceView : public QWidget
{
    Q_OBJECT

public:
    explicit WorkspaceView(QWidget *parent = nullptr);

    void setTemplateContent(const QString &code, const QString &label = QString());
    void setSnippets(const QList<Snippet> &snippets);

private slots:
    void onBrowseDirectory();
    void onBrowseTemplate();
    void onBuildFromSnippets();
    void onClearTemplate();
    void onCreateClicked();
    void onProblemSelected(int row);
    void onSaveClicked();
    void onClearFormClicked();

private:
    void setupUI();
    void populateFileList(const QStringList &letters, const QString &workspacePath);
    void loadFileIntoEditor(const QString &filePath);
    bool saveCurrentFile();
    void setEditorEnabled(bool enabled);
    void setStatusMessage(const QString &msg, bool isError = false);

    QLineEdit   *m_contestNameEdit  = nullptr;
    QSpinBox    *m_problemCountSpin = nullptr;
    QLineEdit   *m_outputDirEdit    = nullptr;
    QPushButton *m_browseDirBtn     = nullptr;
    QPushButton *m_createBtn        = nullptr;
    QPushButton *m_clearFormBtn;
    QLabel      *m_statusLabel      = nullptr;

    QLineEdit   *m_templatePathEdit = nullptr;
    QPushButton *m_browseTemplateBtn = nullptr;
    QPushButton *m_fromSnippetsBtn   = nullptr;
    QPushButton *m_clearTemplateBtn  = nullptr;
    QString      m_inlineTemplateContent;
    QList<Snippet> m_snippets;

    QListWidget *m_fileList         = nullptr;

    QLabel         *m_editorTitleLabel = nullptr;
    QPlainTextEdit *m_editor           = nullptr;
    QPushButton    *m_saveBtn          = nullptr;

    QString m_currentFilePath;
    QString m_workspacePath;

    CppSyntaxHighlighter *m_highlighter = nullptr;
};