#include "snippeteditorview.h"
#include "../editor/Cppsyntaxhighlighter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
#include <QClipboard>
#include <QFont>

SnippetEditorView::SnippetEditorView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SnippetEditorView");
    setupUI();
    clearForm();
}

void SnippetEditorView::setupUI()
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
    m_titleEdit->setPlaceholderText("e.g. Segment Tree with Lazy Propagation");
    form->addRow("Title:", m_titleEdit);

    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->setObjectName("FilterCombo");
    m_categoryCombo->setEditable(true);
    m_categoryCombo->setInsertPolicy(QComboBox::InsertAtBottom);
    m_categoryCombo->setPlaceholderText("e.g. Data Structures");
    form->addRow("Category:", m_categoryCombo);

    m_tagsEdit = new QLineEdit(this);
    m_tagsEdit->setObjectName("FormField");
    m_tagsEdit->setPlaceholderText("Comma-separated, e.g. tree, lazy, range-query");
    form->addRow("Tags:", m_tagsEdit);

    root->addLayout(form);

    QLabel *codeLabel = new QLabel("Code:", this);
    codeLabel->setObjectName("FormLabel");
    root->addWidget(codeLabel);

    m_codeEditor = new QPlainTextEdit(this);
    m_codeEditor->setObjectName("CodeEditor");
    m_codeEditor->setFont(QFont("Monospace", 10));
    m_codeEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_highlighter = new CppSyntaxHighlighter(m_codeEditor->document());
    root->addWidget(m_codeEditor, 1);

    QHBoxLayout *actions = new QHBoxLayout;

    m_copyBtn = new QPushButton("Copy Code", this);
    m_copyBtn->setObjectName("SecondaryButton");
    connect(m_copyBtn, &QPushButton::clicked,
            this,      &SnippetEditorView::onCopyClicked);

    m_saveBtn = new QPushButton("Save", this);
    m_saveBtn->setObjectName("PrimaryButton");
    connect(m_saveBtn, &QPushButton::clicked,
            this,      &SnippetEditorView::onSaveClicked);

    m_clearBtn = new QPushButton("Clear", this);
    m_clearBtn->setObjectName("SecondaryButton");
    connect(m_clearBtn, &QPushButton::clicked, this, &SnippetEditorView::onClearClicked);

    actions->addWidget(m_copyBtn);
    actions->addWidget(m_clearBtn);
    actions->addStretch();
    actions->addWidget(m_saveBtn);
    root->addLayout(actions);
}

void SnippetEditorView::clearForm()
{
    m_currentId = -1;
    m_titleEdit->clear();
    if (m_categoryCombo->isEditable())
        m_categoryCombo->clearEditText();
    else
        m_categoryCombo->setCurrentIndex(-1);
    m_tagsEdit->clear();
    m_codeEditor->clear();
}

void SnippetEditorView::loadSnippet(const Snippet &snippet)
{
    m_currentId = snippet.id;
    m_modeLabel->setText("Edit Snippet");
    m_titleEdit->setText(snippet.title);

    const int idx = m_categoryCombo->findText(snippet.category);
    if (idx >= 0)
        m_categoryCombo->setCurrentIndex(idx);
    else {
        m_categoryCombo->addItem(snippet.category);
        m_categoryCombo->setCurrentText(snippet.category);
    }

    m_tagsEdit->setText(snippet.tags.join(", "));
    m_codeEditor->setPlainText(snippet.code);
}

void SnippetEditorView::setCreateMode()
{
    clearForm();
    m_modeLabel->setText("New Snippet");
}

void SnippetEditorView::setKnownCategories(const QList<QString> &categories)
{
    const QString current = m_categoryCombo->currentText();
    m_categoryCombo->clear();
    for (const QString &cat : categories)
        m_categoryCombo->addItem(cat);
    if (!current.isEmpty())
        m_categoryCombo->setCurrentText(current);
}

Snippet SnippetEditorView::collectSnippet() const
{
    Snippet s;
    s.id       = m_currentId;
    s.title    = m_titleEdit->text().trimmed();
    s.category = m_categoryCombo->currentText().trimmed();
    s.code     = m_codeEditor->toPlainText();

    const QStringList rawTags = m_tagsEdit->text().split(',');
    for (const QString &t : rawTags) {
        const QString trimmed = t.trimmed();
        if (!trimmed.isEmpty())
            s.tags.append(trimmed);
    }
    return s;
}

void SnippetEditorView::onSaveClicked()
{
    const Snippet s = collectSnippet();
    if (s.title.isEmpty())
        return;
    emit saved(s);
    setCreateMode();
}

void SnippetEditorView::onClearClicked()
{
    setCreateMode();
}

void SnippetEditorView::onCopyClicked()
{
    QApplication::clipboard()->setText(m_codeEditor->toPlainText());
    emit copyRequested();
}