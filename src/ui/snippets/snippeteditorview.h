#pragma once

#include "../../models.h"

#include <QWidget>

class QLineEdit;
class QComboBox;
class QPlainTextEdit;
class QPushButton;
class QLabel;
class CppSyntaxHighlighter;

class SnippetEditorView : public QWidget
{
    Q_OBJECT

public:
    explicit SnippetEditorView(QWidget *parent = nullptr);

    void loadSnippet(const Snippet &snippet);
    void setCreateMode();
    void setKnownCategories(const QList<QString> &categories);

signals:
    void saved(const Snippet &snippet);
    void copyRequested();

private slots:
    void onSaveClicked();
    void onCopyClicked();
    void onClearClicked();

private:
    void setupUI();
    void clearForm();
    Snippet collectSnippet() const;

    int m_currentId = -1;

    QLabel         *m_modeLabel;
    QLineEdit      *m_titleEdit;
    QComboBox      *m_categoryCombo;
    QLineEdit      *m_tagsEdit;
    QPlainTextEdit *m_codeEditor;
    QPushButton    *m_saveBtn;
    QPushButton    *m_copyBtn;
    QPushButton *m_clearBtn;

    CppSyntaxHighlighter *m_highlighter = nullptr;
};