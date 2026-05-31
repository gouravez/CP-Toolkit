#pragma once

#include "../../models.h"

#include <QWidget>
#include <QList>

class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QPushButton;
class QLabel;
class CppSyntaxHighlighter;

class TemplateView : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateView(QWidget *parent = nullptr);

    void setSnippets(const QList<Snippet> &snippets);

private slots:
    void onSelectionChanged(QListWidgetItem *item);
    void onExportClicked();
    void onSelectAllClicked();
    void onClearSelectionClicked();

private:
    void setupUI();
    void regeneratePreview();
    QList<Snippet> selectedSnippets() const;

    QListWidget    *m_snippetList   = nullptr;
    QPlainTextEdit *m_preview       = nullptr;
    QPushButton    *m_exportBtn     = nullptr;
    QPushButton    *m_selectAllBtn  = nullptr;
    QPushButton    *m_clearBtn      = nullptr;
    QLabel         *m_statusLabel   = nullptr;

    CppSyntaxHighlighter *m_highlighter = nullptr;

    QList<Snippet> m_snippets;
};