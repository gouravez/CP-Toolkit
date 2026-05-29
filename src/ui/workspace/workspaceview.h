#pragma once

#include <QWidget>

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

private slots:
    void onBrowseDirectory();
    void onCreateClicked();
    void onProblemSelected(int row);
    void onSaveClicked();

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
    QLabel      *m_statusLabel      = nullptr;

    QListWidget *m_fileList         = nullptr;

    QLabel         *m_editorTitleLabel = nullptr;
    QPlainTextEdit *m_editor           = nullptr;
    QPushButton    *m_saveBtn          = nullptr;

    QString m_currentFilePath;
    QString m_workspacePath;

    CppSyntaxHighlighter *m_highlighter = nullptr;
};