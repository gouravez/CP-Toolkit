#include "searchview.h"
#include "../../core/searchindex.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>

SearchView::SearchView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SearchView");
    setupUI();
}

void SearchView::setupUI()
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    QLabel *header = new QLabel(tr("Search"), this);
    header->setObjectName("PanelTitle");
    root->addWidget(header);

    m_searchBar = new QLineEdit(this);
    m_searchBar->setObjectName("ListSearchBar");
    m_searchBar->setPlaceholderText(
        tr("Search snippets and solutions by title, tag, topic, or platform... (Ctrl+K)"));
    m_searchBar->setClearButtonEnabled(true);
    root->addWidget(m_searchBar);

    m_resultCountLabel = new QLabel(this);
    m_resultCountLabel->setObjectName("StatusLabel");
    root->addWidget(m_resultCountLabel);

    m_resultList = new QListWidget(this);
    m_resultList->setObjectName("ItemList");
    m_resultList->setSpacing(2);
    root->addWidget(m_resultList, 1);

    connect(m_searchBar, &QLineEdit::textChanged,
            this,        &SearchView::onQueryChanged);

    connect(m_resultList, &QListWidget::itemActivated,
            this,         &SearchView::onItemActivated);
}

void SearchView::setSearchIndex(SearchIndex *index)
{
    m_index = index;
    runQuery(m_searchBar->text());
}

void SearchView::refresh()
{
    runQuery(m_searchBar->text());
}

void SearchView::focusSearchBar()
{
    m_searchBar->setFocus(Qt::ShortcutFocusReason);
    m_searchBar->selectAll();
}

void SearchView::clear()
{
    m_searchBar->clear();
}

void SearchView::onQueryChanged(const QString &text)
{
    runQuery(text);
}

void SearchView::runQuery(const QString &text)
{
    m_resultList->clear();

    if (!m_index) {
        m_resultCountLabel->setText(tr("Search index not ready."));
        return;
    }

    const QList<SearchResult> results = m_index->search(text);

    for (const SearchResult &r : results) {
        const bool isSnippet = (r.type == SearchResult::Type::Snippet);
        const QString typeLabel = isSnippet ? tr("Snippet") : tr("Solution");
        const QString label = r.subtitle.isEmpty()
            ? QStringLiteral("%1   [%2]").arg(r.title, typeLabel)
            : QStringLiteral("%1   [%2 \u00B7 %3]").arg(r.title, typeLabel, r.subtitle);

        QListWidgetItem *item = new QListWidgetItem(m_resultList);
        item->setText(label);
        item->setData(Qt::UserRole,     r.id);
        item->setData(Qt::UserRole + 1, isSnippet ? 0 : 1);
    }

    if (text.trimmed().isEmpty())
        m_resultCountLabel->setText(tr("%1 total record(s)").arg(results.size()));
    else
        m_resultCountLabel->setText(tr("%1 result(s) for \"%2\"").arg(results.size()).arg(text));
}

void SearchView::onItemActivated(QListWidgetItem *item)
{
    if (!item) return;

    const int id       = item->data(Qt::UserRole).toInt();
    const int typeFlag  = item->data(Qt::UserRole + 1).toInt();

    if (typeFlag == 0)
        emit snippetActivated(id);
    else
        emit solutionActivated(id);
}