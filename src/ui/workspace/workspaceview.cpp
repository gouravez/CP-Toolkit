#include "workspaceview.h"
#include "../editor/Cppsyntaxhighlighter.h"
#include "../../core/workspacegenerator.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTextStream>
#include <QVBoxLayout>

WorkspaceView::WorkspaceView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("WorkspaceView"));
    setupUI();
}

void WorkspaceView::setupUI()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    QLabel *header = new QLabel(tr("Contest Workspace"), this);
    header->setObjectName(QStringLiteral("PanelTitle"));
    root->addWidget(header);

    QGroupBox *formBox = new QGroupBox(tr("New Workspace"), this);
    formBox->setObjectName(QStringLiteral("FormBox"));
    QVBoxLayout *formVLayout = new QVBoxLayout(formBox);
    formVLayout->setSpacing(8);

    QHBoxLayout *nameRow = new QHBoxLayout;
    QLabel *nameLabel = new QLabel(tr("Contest name:"), formBox);
    nameLabel->setFixedWidth(110);
    m_contestNameEdit = new QLineEdit(formBox);
    m_contestNameEdit->setPlaceholderText(tr("e.g. Codeforces Round 999"));
    m_contestNameEdit->setObjectName(QStringLiteral("FormLineEdit"));
    nameRow->addWidget(nameLabel);
    nameRow->addWidget(m_contestNameEdit, 1);
    formVLayout->addLayout(nameRow);

    QHBoxLayout *countRow = new QHBoxLayout;
    QLabel *countLabel = new QLabel(tr("Problems (A–?):"), formBox);
    countLabel->setFixedWidth(110);
    m_problemCountSpin = new QSpinBox(formBox);
    m_problemCountSpin->setRange(1, 26);
    m_problemCountSpin->setValue(6);
    m_problemCountSpin->setObjectName(QStringLiteral("FormSpinBox"));
    m_problemCountSpin->setFixedWidth(70);
    countRow->addWidget(countLabel);
    countRow->addWidget(m_problemCountSpin);
    countRow->addStretch();
    formVLayout->addLayout(countRow);

    QHBoxLayout *dirRow = new QHBoxLayout;
    QLabel *dirLabel = new QLabel(tr("Output folder:"), formBox);
    dirLabel->setFixedWidth(110);
    m_outputDirEdit = new QLineEdit(formBox);
    m_outputDirEdit->setObjectName(QStringLiteral("FormLineEdit"));
    m_outputDirEdit->setText(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    m_browseDirBtn = new QPushButton(tr("Browse…"), formBox);
    m_browseDirBtn->setObjectName(QStringLiteral("SecondaryButton"));
    m_browseDirBtn->setCursor(Qt::PointingHandCursor);
    m_browseDirBtn->setFixedWidth(80);
    dirRow->addWidget(dirLabel);
    dirRow->addWidget(m_outputDirEdit, 1);
    dirRow->addWidget(m_browseDirBtn);
    formVLayout->addLayout(dirRow);

    QHBoxLayout *actionRow = new QHBoxLayout;
    m_createBtn = new QPushButton(tr("Create Workspace"), formBox);
    m_createBtn->setObjectName(QStringLiteral("PrimaryButton"));
    m_createBtn->setCursor(Qt::PointingHandCursor);
    m_createBtn->setFixedWidth(150);

    m_clearFormBtn = new QPushButton(tr("Clear"), formBox);
    m_clearFormBtn->setObjectName(QStringLiteral("SecondaryButton"));
    m_clearFormBtn->setCursor(Qt::PointingHandCursor);
    m_clearFormBtn->setFixedWidth(80);
    connect(m_clearFormBtn, &QPushButton::clicked, this, &WorkspaceView::onClearFormClicked);

    m_statusLabel = new QLabel(formBox);
    m_statusLabel->setObjectName(QStringLiteral("StatusLabel"));
    m_statusLabel->setWordWrap(true);

    actionRow->addWidget(m_createBtn);
    actionRow->addWidget(m_clearFormBtn);
    actionRow->addWidget(m_statusLabel, 1);
    formVLayout->addLayout(actionRow);

    root->addWidget(formBox);

    QHBoxLayout *workArea = new QHBoxLayout;
    workArea->setSpacing(10);

    QGroupBox *listBox = new QGroupBox(tr("Problem Files"), this);
    listBox->setObjectName(QStringLiteral("SourcePaneBox"));
    listBox->setFixedWidth(150);
    QVBoxLayout *listLayout = new QVBoxLayout(listBox);
    listLayout->setContentsMargins(6, 6, 6, 6);
    m_fileList = new QListWidget(listBox);
    m_fileList->setObjectName(QStringLiteral("ProblemFileList"));
    m_fileList->setCursor(Qt::PointingHandCursor);
    listLayout->addWidget(m_fileList);
    workArea->addWidget(listBox);

    QGroupBox *editorBox = new QGroupBox(this);
    editorBox->setObjectName(QStringLiteral("SourcePaneBox"));
    QVBoxLayout *editorLayout = new QVBoxLayout(editorBox);
    editorLayout->setContentsMargins(6, 6, 6, 6);
    editorLayout->setSpacing(6);

    QHBoxLayout *editorHeader = new QHBoxLayout;
    m_editorTitleLabel = new QLabel(tr("No file open"), editorBox);
    m_editorTitleLabel->setObjectName(QStringLiteral("PathLabel"));
    m_saveBtn = new QPushButton(tr("Save"), editorBox);
    m_saveBtn->setObjectName(QStringLiteral("SecondaryButton"));
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setFixedWidth(60);
    m_saveBtn->setEnabled(false);
    editorHeader->addWidget(m_editorTitleLabel, 1);
    editorHeader->addWidget(m_saveBtn);
    editorLayout->addLayout(editorHeader);

    m_editor = new QPlainTextEdit(editorBox);
    m_editor->setObjectName(QStringLiteral("CodeEditor"));
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_editor->setEnabled(false);
    m_editor->setPlaceholderText(
        tr("Create a workspace and select a problem file to start editing."));

    QFont monoFont(QStringLiteral("Consolas"));
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setPointSize(10);
    m_editor->setFont(monoFont);

    m_highlighter = new CppSyntaxHighlighter(m_editor->document());

    editorLayout->addWidget(m_editor, 1);
    workArea->addWidget(editorBox, 1);

    root->addLayout(workArea, 1);

    connect(m_browseDirBtn, &QPushButton::clicked,
            this, &WorkspaceView::onBrowseDirectory);
    connect(m_createBtn, &QPushButton::clicked,
            this, &WorkspaceView::onCreateClicked);
    connect(m_fileList, &QListWidget::currentRowChanged,
            this, &WorkspaceView::onProblemSelected);
    connect(m_saveBtn, &QPushButton::clicked,
            this, &WorkspaceView::onSaveClicked);
}

void WorkspaceView::onBrowseDirectory()
{
    const QString dir = QFileDialog::getExistingDirectory(
        this, tr("Select Output Folder"), m_outputDirEdit->text());

    if (!dir.isEmpty())
        m_outputDirEdit->setText(dir);
}

void WorkspaceView::onCreateClicked()
{
    const QString contestName = m_contestNameEdit->text().trimmed();
    if (contestName.isEmpty()) {
        setStatusMessage(tr("Please enter a contest name."), true);
        m_contestNameEdit->setFocus();
        return;
    }

    const QString baseDir = m_outputDirEdit->text().trimmed();
    if (baseDir.isEmpty() || !QDir(baseDir).exists()) {
        setStatusMessage(tr("Output folder does not exist. Please browse for a valid folder."), true);
        return;
    }

    saveCurrentFile();

    WorkspaceGenerator gen;
    gen.setBaseDirectory(baseDir);
    gen.setContestName(contestName);
    gen.setProblemCount(m_problemCountSpin->value());

    if (!gen.create()) {
        setStatusMessage(gen.errorString(), true);
        return;
    }

    m_workspacePath = gen.workspacePath();

    const QList<QString> letters = gen.problemLetters();
    QStringList letterList;
    for (const QString &l : letters)
        letterList.append(l);

    populateFileList(letterList, m_workspacePath);

    setStatusMessage(
        tr("Workspace created: %1").arg(QDir::toNativeSeparators(m_workspacePath)));

    if (!letterList.isEmpty())
        m_fileList->setCurrentRow(0);
}

void WorkspaceView::onProblemSelected(int row)
{
    if (row < 0 || m_workspacePath.isEmpty())
        return;

    saveCurrentFile();

    const QString letter = m_fileList->item(row)->text();
    const QString filePath = QDir(m_workspacePath).filePath(letter);
    loadFileIntoEditor(filePath);
}

void WorkspaceView::onSaveClicked()
{
    if (saveCurrentFile())
        setStatusMessage(tr("Saved %1").arg(QFileInfo(m_currentFilePath).fileName()));
}

void WorkspaceView::populateFileList(const QStringList &letters,
                                      const QString &workspacePath)
{
    Q_UNUSED(workspacePath)

    m_fileList->clear();
    m_currentFilePath.clear();
    m_editor->clear();
    setEditorEnabled(false);
    m_editorTitleLabel->setText(tr("No file open"));

    for (const QString &letter : letters)
        m_fileList->addItem(letter + QStringLiteral(".cpp"));
}

void WorkspaceView::loadFileIntoEditor(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStatusMessage(tr("Could not open file: %1").arg(filePath), true);
        return;
    }

    QTextStream ts(&file);
    const QString content = ts.readAll();

    m_currentFilePath = filePath;
    m_editor->setPlainText(content);
    m_editorTitleLabel->setText(QDir::toNativeSeparators(filePath));
    setEditorEnabled(true);
}

bool WorkspaceView::saveCurrentFile()
{
    if (m_currentFilePath.isEmpty())
        return true;

    QFile file(m_currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setStatusMessage(tr("Could not save file: %1").arg(m_currentFilePath), true);
        return false;
    }

    QTextStream ts(&file);
    ts << m_editor->toPlainText();
    return true;
}

void WorkspaceView::setEditorEnabled(bool enabled)
{
    m_editor->setEnabled(enabled);
    m_saveBtn->setEnabled(enabled);
}

void WorkspaceView::setStatusMessage(const QString &msg, bool isError)
{
    m_statusLabel->setText(msg);
    m_statusLabel->setProperty("error", isError);
    m_statusLabel->style()->unpolish(m_statusLabel);
    m_statusLabel->style()->polish(m_statusLabel);
}

void WorkspaceView::onClearFormClicked()
{
    m_contestNameEdit->clear();
    m_problemCountSpin->setValue(6);
    setStatusMessage("");
}