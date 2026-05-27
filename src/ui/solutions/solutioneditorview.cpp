#include "solutioneditorview.h"
#include "../editor/Cppsyntaxhighlighter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QFont>

SolutionEditorView::SolutionEditorView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SolutionEditorView");
    setupUI();
    clearForm();
}

void SolutionEditorView::setupUI()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    m_modeLabel = new QLabel(this);
    m_modeLabel->setObjectName("PanelTitle");
    root->addWidget(m_modeLabel);

    QFormLayout *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    form->setHorizontalSpacing(12);
    form->setVerticalSpacing(8);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setObjectName("FormField");
    m_titleEdit->setPlaceholderText("e.g. Two Sum");
    form->addRow("Title:", m_titleEdit);

    m_platformCombo = new QComboBox(this);
    m_platformCombo->setObjectName("FilterCombo");
    m_platformCombo->setEditable(true);
    m_platformCombo->setInsertPolicy(QComboBox::InsertAtBottom);
    m_platformCombo->setPlaceholderText("e.g. Codeforces");
    for (const QString &p : {"Codeforces", "AtCoder", "CSES", "LeetCode", "USACO"})
        m_platformCombo->addItem(p);
    form->addRow("Platform:", m_platformCombo);

    m_difficultyEdit = new QSpinBox(this);
    m_difficultyEdit->setObjectName("SmallSpinBox");
    m_difficultyEdit->setRange(0, 9999);
    m_difficultyEdit->setSpecialValueText("(unrated)");
    form->addRow("Difficulty:", m_difficultyEdit);

    m_urlEdit = new QLineEdit(this);
    m_urlEdit->setObjectName("FormField");
    m_urlEdit->setPlaceholderText("https://codeforces.com/problemset/problem/...");
    form->addRow("Problem URL:", m_urlEdit);

    m_topicsEdit = new QLineEdit(this);
    m_topicsEdit->setObjectName("FormField");
    m_topicsEdit->setPlaceholderText("Comma-separated, e.g. graphs, dfs, dp");
    form->addRow("Topics:", m_topicsEdit);

    root->addLayout(form);

    QLabel *codeLabel = new QLabel("Solution code:", this);
    codeLabel->setObjectName("FormLabel");
    root->addWidget(codeLabel);

    m_codeEditor = new QPlainTextEdit(this);
    m_codeEditor->setObjectName("CodeEditor");
    m_codeEditor->setFont(QFont("Monospace", 10));
    m_codeEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_highlighter = new CppSyntaxHighlighter(m_codeEditor->document());
    root->addWidget(m_codeEditor, 1);

    QHBoxLayout *actions = new QHBoxLayout;

    m_saveBtn = new QPushButton("Save", this);
    m_saveBtn->setObjectName("PrimaryButton");
    connect(m_saveBtn, &QPushButton::clicked,
            this,      &SolutionEditorView::onSaveClicked);

    actions->addStretch();
    actions->addWidget(m_saveBtn);
    root->addLayout(actions);
}

void SolutionEditorView::clearForm()
{
    m_currentId = -1;
    m_titleEdit->clear();
    m_platformCombo->setCurrentIndex(-1);
    if (m_platformCombo->isEditable())
        m_platformCombo->clearEditText();
    m_difficultyEdit->setValue(0);
    m_urlEdit->clear();
    m_topicsEdit->clear();
    m_codeEditor->clear();
}

void SolutionEditorView::loadSolution(const Solution &solution)
{
    m_currentId = solution.id;
    m_modeLabel->setText("Edit Solution");
    m_titleEdit->setText(solution.title);

    const int idx = m_platformCombo->findText(solution.platform);
    if (idx >= 0)
        m_platformCombo->setCurrentIndex(idx);
    else {
        m_platformCombo->addItem(solution.platform);
        m_platformCombo->setCurrentText(solution.platform);
    }

    m_difficultyEdit->setValue(solution.difficulty);
    m_urlEdit->setText(solution.problemUrl);
    m_topicsEdit->setText(solution.topics.join(", "));
    m_codeEditor->setPlainText(solution.code);
}

void SolutionEditorView::setCreateMode()
{
    clearForm();
    m_modeLabel->setText("New Solution");
}

void SolutionEditorView::setKnownPlatforms(const QList<QString> &platforms)
{
    const QString current = m_platformCombo->currentText();
    m_platformCombo->clear();
    for (const QString &p : platforms)
        m_platformCombo->addItem(p);
    if (!current.isEmpty())
        m_platformCombo->setCurrentText(current);
}

Solution SolutionEditorView::collectSolution() const
{
    Solution s;
    s.id         = m_currentId;
    s.title      = m_titleEdit->text().trimmed();
    s.platform   = m_platformCombo->currentText().trimmed();
    s.difficulty = m_difficultyEdit->value();
    s.problemUrl = m_urlEdit->text().trimmed();
    s.code       = m_codeEditor->toPlainText();

    const QStringList rawTopics = m_topicsEdit->text().split(',');
    for (const QString &t : rawTopics) {
        const QString trimmed = t.trimmed();
        if (!trimmed.isEmpty())
            s.topics.append(trimmed);
    }
    return s;
}

void SolutionEditorView::onSaveClicked()
{
    const Solution s = collectSolution();
    if (s.title.isEmpty())
        return;
    emit saved(s);
}