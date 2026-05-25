#pragma once

#include "../../models.h"

#include <QWidget>
#include <QList>

class QListWidget;
class QListWidgetItem;
class QPushButton;
class QComboBox;
class QLineEdit;

class SnippetListView : public QWidget
{
    Q_OBJECT

public:
    explicit SnippetListView(QWidget *parent = nullptr);

    void setSnippets(const QList<Snippet> &snippets);
    void setCategories(const QList<QString> &categories);

signals:
    void snippetSelected(int id);
    void createRequested();
    void deleteRequested(int id);

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onFilterChanged();
    void onDeleteClicked();

private:
    void setupUI();
    void applyFilter();

    QLineEdit   *m_searchBar;
    QComboBox   *m_categoryFilter;
    QListWidget *m_list;
    QPushButton *m_newBtn;
    QPushButton *m_deleteBtn;

    QList<Snippet> m_snippets;
};