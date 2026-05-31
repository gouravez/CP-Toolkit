#include "templateview.h"
#include "../editor/Cppsyntaxhighlighter.h"
#include "../../core/templategenerator.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFont>

TemplateView::TemplateView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("TemplateView");
    setupUI();
}

void TemplateView::setupUI()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QWidget *toolbar = new QWidget(this);
    toolbar->setObjectName("ViewToolbar");
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(12, 8, 12, 8);
    toolbarLayout->setSpacing(8);

    QLabel *title = new QLabel("Template Generator", toolbar);
    title->setObjectName("PanelTitle");

    m_selectAllBtn = new QPushButton("Select All", toolbar);
    m_selectAllBtn->setObjectName("SecondaryButton");

    m_clearBtn = new QPushButton("Clear", toolbar);
    m_clearBtn->setObjectName("SecondaryButton");

    m_exportBtn = new QPushButton("Export .cpp", toolbar);
    m_exportBtn->setObjectName("PrimaryButton");
    m_exportBtn->setEnabled(false);

    m_statusLabel = new QLabel(toolbar);
    m_statusLabel->setObjectName("StatusLabel");

    toolbarLayout->addWidget(title);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_statusLabel);
    toolbarLayout->addWidget(m_selectAllBtn);
    toolbarLayout->addWidget(m_clearBtn);
    toolbarLayout->addWidget(m_exportBtn);
    root->addWidget(toolbar);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);

    QWidget *leftPanel = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(8, 8, 8, 8);
    leftLayout->setSpacing(6);

    QLabel *listLabel = new QLabel("Available Snippets", leftPanel);
    listLabel->setObjectName("FormLabel");
    leftLayout->addWidget(listLabel);

    m_snippetList = new QListWidget(leftPanel);
    m_snippetList->setObjectName("ItemList");
    m_snippetList->setSpacing(2);
    leftLayout->addWidget(m_snippetList, 1);

    splitter->addWidget(leftPanel);

    QWidget *rightPanel = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(8, 8, 8, 8);
    rightLayout->setSpacing(6);

    QLabel *previewLabel = new QLabel("Generated Template Preview", rightPanel);
    previewLabel->setObjectName("FormLabel");
    rightLayout->addWidget(previewLabel);

    m_preview = new QPlainTextEdit(rightPanel);
    m_preview->setObjectName("CodeEditor");
    m_preview->setFont(QFont("Monospace", 10));
    m_preview->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_preview->setReadOnly(true);
    m_highlighter = new CppSyntaxHighlighter(m_preview->document());
    rightLayout->addWidget(m_preview, 1);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    root->addWidget(splitter, 1);

    connect(m_snippetList, &QListWidget::itemChanged,
            this,          &TemplateView::onSelectionChanged);

    connect(m_exportBtn,   &QPushButton::clicked,
            this,          &TemplateView::onExportClicked);

    connect(m_selectAllBtn, &QPushButton::clicked,
            this,           &TemplateView::onSelectAllClicked);

    connect(m_clearBtn,    &QPushButton::clicked,
            this,          &TemplateView::onClearSelectionClicked);
}

void TemplateView::setSnippets(const QList<Snippet> &snippets)
{
    m_snippets = snippets;

    m_snippetList->blockSignals(true);
    m_snippetList->clear();

    for (const Snippet &s : snippets) {
        QListWidgetItem *item = new QListWidgetItem(s.title, m_snippetList);
        item->setData(Qt::UserRole, s.id);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        if (!s.category.isEmpty())
            item->setToolTip(s.category);
    }

    m_snippetList->blockSignals(false);
    regeneratePreview();
}

void TemplateView::onSelectionChanged(QListWidgetItem * /*item*/)
{
    regeneratePreview();
}

void TemplateView::regeneratePreview()
{
    const QList<Snippet> selected = selectedSnippets();

    if (selected.isEmpty()) {
        m_preview->setPlainText("// Select snippets on the left to generate a template.");
        m_exportBtn->setEnabled(false);
        m_statusLabel->clear();
        return;
    }

    TemplateGenerator gen;
    gen.setSnippets(selected);
    m_preview->setPlainText(gen.generate());
    m_exportBtn->setEnabled(true);
    m_statusLabel->setText(QString("%1 snippet(s) selected").arg(selected.size()));
}

QList<Snippet> TemplateView::selectedSnippets() const
{
    QList<Snippet> result;
    for (int i = 0; i < m_snippetList->count(); ++i) {
        QListWidgetItem *item = m_snippetList->item(i);
        if (item->checkState() == Qt::Checked) {
            const int id = item->data(Qt::UserRole).toInt();
            for (const Snippet &s : m_snippets) {
                if (s.id == id) {
                    result.append(s);
                    break;
                }
            }
        }
    }
    return result;
}

void TemplateView::onExportClicked()
{
    const QList<Snippet> selected = selectedSnippets();
    if (selected.isEmpty())
        return;

    const QString path = QFileDialog::getSaveFileName(
        this, "Export Template", "template.cpp",
        "C++ Source Files (*.cpp);;All Files (*)");

    if (path.isEmpty())
        return;

    TemplateGenerator gen;
    gen.setSnippets(selected);

    if (gen.exportToFile(path))
        m_statusLabel->setText("Exported to " + path);
    else
        m_statusLabel->setText("Export failed — check permissions.");
}

void TemplateView::onSelectAllClicked()
{
    m_snippetList->blockSignals(true);
    for (int i = 0; i < m_snippetList->count(); ++i)
        m_snippetList->item(i)->setCheckState(Qt::Checked);
    m_snippetList->blockSignals(false);
    regeneratePreview();
}

void TemplateView::onClearSelectionClicked()
{
    m_snippetList->blockSignals(true);
    for (int i = 0; i < m_snippetList->count(); ++i)
        m_snippetList->item(i)->setCheckState(Qt::Unchecked);
    m_snippetList->blockSignals(false);
    regeneratePreview();
}