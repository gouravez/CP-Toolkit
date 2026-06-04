#pragma once

#include "../../models.h"

#include <QWidget>

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QLabel;
class SearchIndex;

class SearchView : public QWidget
{
    Q_OBJECT

public:
    explicit SearchView(QWidget *parent = nullptr);

    void setSearchIndex(SearchIndex *index);
    void refresh();
    void focusSearchBar();
    void clear();

signals:
    void snippetActivated(int id);
    void solutionActivated(int id);

private slots:
    void onQueryChanged(const QString &text);
    void onItemActivated(QListWidgetItem *item);

private:
    void setupUI();
    void runQuery(const QString &text);

    SearchIndex *m_index = nullptr;

    QLineEdit   *m_searchBar  = nullptr;
    QListWidget *m_resultList = nullptr;
    QLabel      *m_resultCountLabel = nullptr;
};