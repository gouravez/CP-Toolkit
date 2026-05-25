#include "snippetlistview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

SnippetListView::SnippetListView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SnippetListView");
    setFixedWidth(260);
    setupUI();
}

void SnippetListView::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    QLabel *title = new QLabel("Snippets", this);
    title->setObjectName("PanelTitle");

    m_newBtn = new QPushButton("+ New", this);
    m_newBtn->setObjectName("PrimaryButton");
    m_newBtn->setFixedWidth(60);

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(m_newBtn);
    layout->addLayout(headerLayout);

    m_searchBar = new QLineEdit(this);
    m_searchBar->setPlaceholderText("Filter snippets...");
    m_searchBar->setObjectName("ListSearchBar");
    layout->addWidget(m_searchBar);

    m_categoryFilter = new QComboBox(this);
    m_categoryFilter->setObjectName("FilterCombo");
    m_categoryFilter->addItem("All Categories");
    layout->addWidget(m_categoryFilter);

    m_list = new QListWidget(this);
    m_list->setObjectName("ItemList");
    m_list->setSpacing(2);
    layout->addWidget(m_list, 1);

    m_deleteBtn = new QPushButton("Delete Selected", this);
    m_deleteBtn->setObjectName("DangerButton");
    m_deleteBtn->setEnabled(false);
    layout->addWidget(m_deleteBtn);

    connect(m_newBtn,    &QPushButton::clicked,
            this,        &SnippetListView::createRequested);

    connect(m_list,      &QListWidget::itemClicked,
            this,        &SnippetListView::onItemClicked);

    connect(m_searchBar, &QLineEdit::textChanged,
            this,        &SnippetListView::onFilterChanged);

    connect(m_categoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,             &SnippetListView::onFilterChanged);

    connect(m_deleteBtn, &QPushButton::clicked,
            this,        &SnippetListView::onDeleteClicked);
}

void SnippetListView::setSnippets(const QList<Snippet> &snippets)
{
    m_snippets = snippets;
    m_list->clear();
    for (const Snippet &s : m_snippets) {
        QListWidgetItem *item = new QListWidgetItem(m_list);
        item->setText(s.title);
        item->setData(Qt::UserRole, s.id);
        if (!s.tags.isEmpty())
            item->setToolTip(s.tags.join(", "));
    }
    applyFilter();
}

void SnippetListView::setCategories(const QList<QString> &categories)
{
    m_categoryFilter->blockSignals(true);
    m_categoryFilter->clear();
    m_categoryFilter->addItem("All Categories");
    for (const QString &cat : categories)
        m_categoryFilter->addItem(cat);
    m_categoryFilter->blockSignals(false);
}

void SnippetListView::applyFilter()
{
    const QString text     = m_searchBar->text().toLower();
    const QString category = m_categoryFilter->currentIndex() == 0
                             ? QString()
                             : m_categoryFilter->currentText();
    m_deleteBtn->setEnabled(false);

    for (int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem *item = m_list->item(i);
        const int id = item->data(Qt::UserRole).toInt();

        auto it = std::find_if(m_snippets.begin(), m_snippets.end(),
                               [id](const Snippet &s){ return s.id == id; });
        if (it == m_snippets.end()) continue;

        const Snippet &s = *it;
        bool hide = false;

        if (!category.isEmpty() && s.category != category)
            hide = true;

        if (!hide && !text.isEmpty()) {
            bool match = s.title.toLower().contains(text);
            if (!match)
                for (const QString &tag : s.tags)
                    if (tag.toLower().contains(text)) { match = true; break; }
            if (!match) hide = true;
        }

        item->setHidden(hide);
    }
}

void SnippetListView::onItemClicked(QListWidgetItem *item)
{
    if (!item) return;
    m_deleteBtn->setEnabled(true);
    emit snippetSelected(item->data(Qt::UserRole).toInt());
}

void SnippetListView::onFilterChanged() { applyFilter(); }

void SnippetListView::onDeleteClicked()
{
    QListWidgetItem *item = m_list->currentItem();
    if (!item) return;
    emit deleteRequested(item->data(Qt::UserRole).toInt());
}