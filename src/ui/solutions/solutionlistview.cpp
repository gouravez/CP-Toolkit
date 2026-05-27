#include "solutionlistview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <algorithm>

SolutionListView::SolutionListView(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SolutionListView");
    setFixedWidth(280);
    setupUI();
}

void SolutionListView::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    QHBoxLayout *header = new QHBoxLayout;
    QLabel *title = new QLabel("Solutions", this);
    title->setObjectName("PanelTitle");

    m_newBtn = new QPushButton("+ New", this);
    m_newBtn->setObjectName("PrimaryButton");
    m_newBtn->setFixedWidth(60);

    header->addWidget(title);
    header->addStretch();
    header->addWidget(m_newBtn);
    layout->addLayout(header);

    m_searchBar = new QLineEdit(this);
    m_searchBar->setObjectName("ListSearchBar");
    m_searchBar->setPlaceholderText("Filter solutions...");
    layout->addWidget(m_searchBar);

    m_platformFilter = new QComboBox(this);
    m_platformFilter->setObjectName("FilterCombo");
    m_platformFilter->addItem("All Platforms");
    layout->addWidget(m_platformFilter);

    QHBoxLayout *diffRow = new QHBoxLayout;
    diffRow->setSpacing(4);
    diffRow->addWidget(new QLabel("Diff:", this));

    m_minDiff = new QSpinBox(this);
    m_minDiff->setObjectName("SmallSpinBox");
    m_minDiff->setRange(0, 9999);
    m_minDiff->setValue(0);
    m_minDiff->setFixedWidth(65);

    m_maxDiff = new QSpinBox(this);
    m_maxDiff->setObjectName("SmallSpinBox");
    m_maxDiff->setRange(0, 9999);
    m_maxDiff->setValue(9999);
    m_maxDiff->setFixedWidth(65);

    diffRow->addWidget(m_minDiff);
    diffRow->addWidget(new QLabel("–", this));
    diffRow->addWidget(m_maxDiff);
    diffRow->addStretch();
    layout->addLayout(diffRow);

    m_list = new QListWidget(this);
    m_list->setObjectName("ItemList");
    m_list->setSpacing(2);
    layout->addWidget(m_list, 1);

    m_deleteBtn = new QPushButton("Delete Selected", this);
    m_deleteBtn->setObjectName("DangerButton");
    m_deleteBtn->setEnabled(false);
    layout->addWidget(m_deleteBtn);

    connect(m_newBtn,    &QPushButton::clicked,
            this,        &SolutionListView::createRequested);

    connect(m_list,      &QListWidget::itemClicked,
            this,        &SolutionListView::onItemClicked);

    connect(m_searchBar, &QLineEdit::textChanged,
            this,        &SolutionListView::onFilterChanged);

    connect(m_platformFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,             &SolutionListView::onFilterChanged);

    connect(m_minDiff, QOverload<int>::of(&QSpinBox::valueChanged),
            this,      &SolutionListView::onFilterChanged);

    connect(m_maxDiff, QOverload<int>::of(&QSpinBox::valueChanged),
            this,      &SolutionListView::onFilterChanged);

    connect(m_deleteBtn, &QPushButton::clicked,
            this,        &SolutionListView::onDeleteClicked);
}

void SolutionListView::setSolutions(const QList<Solution> &solutions)
{
    m_solutions = solutions;
    m_list->clear();
    for (const Solution &s : m_solutions) {
        QListWidgetItem *item = new QListWidgetItem(m_list);
        const QString label = QString("%1  [%2 | %3]")
                                  .arg(s.title, s.platform)
                                  .arg(s.difficulty);
        item->setText(label);
        item->setData(Qt::UserRole, s.id);
        if (!s.topics.isEmpty())
            item->setToolTip(s.topics.join(", "));
    }
    applyFilter();
}

void SolutionListView::setPlatforms(const QList<QString> &platforms)
{
    m_platformFilter->blockSignals(true);
    m_platformFilter->clear();
    m_platformFilter->addItem("All Platforms");
    for (const QString &p : platforms)
        m_platformFilter->addItem(p);
    m_platformFilter->blockSignals(false);
}

void SolutionListView::applyFilter()
{
    const QString text     = m_searchBar->text().toLower();
    const QString platform = m_platformFilter->currentIndex() == 0
                             ? QString()
                             : m_platformFilter->currentText();
    const int minD = m_minDiff->value();
    const int maxD = m_maxDiff->value();

    m_deleteBtn->setEnabled(false);

    for (int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem *item = m_list->item(i);
        const int id = item->data(Qt::UserRole).toInt();

        auto it = std::find_if(m_solutions.begin(), m_solutions.end(),
                               [id](const Solution &s){ return s.id == id; });
        if (it == m_solutions.end()) continue;

        const Solution &s = *it;
        bool hide = false;

        if (!platform.isEmpty() && s.platform != platform)
            hide = true;

        if (!hide && (s.difficulty < minD || s.difficulty > maxD))
            hide = true;

        if (!hide && !text.isEmpty()) {
            bool match = s.title.toLower().contains(text);
            if (!match)
                for (const QString &topic : s.topics)
                    if (topic.toLower().contains(text)) { match = true; break; }
            if (!match) hide = true;
        }

        item->setHidden(hide);
    }
}

void SolutionListView::onItemClicked(QListWidgetItem *item)
{
    if (!item) return;
    m_deleteBtn->setEnabled(true);
    emit solutionSelected(item->data(Qt::UserRole).toInt());
}

void SolutionListView::onFilterChanged() { applyFilter(); }

void SolutionListView::onDeleteClicked()
{
    QListWidgetItem *item = m_list->currentItem();
    if (!item) return;
    emit deleteRequested(item->data(Qt::UserRole).toInt());
}