#pragma once

#include "../../models.h"

#include <QWidget>
#include <QList>

class QListWidget;
class QListWidgetItem;
class QPushButton;
class QComboBox;
class QLineEdit;
class QSpinBox;

class SolutionListView : public QWidget
{
    Q_OBJECT

public:
    explicit SolutionListView(QWidget *parent = nullptr);

    void setSolutions(const QList<Solution> &solutions);
    void setPlatforms(const QList<QString> &platforms);

signals:
    void solutionSelected(int id);
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
    QComboBox   *m_platformFilter;
    QSpinBox    *m_minDiff;
    QSpinBox    *m_maxDiff;
    QListWidget *m_list;
    QPushButton *m_newBtn;
    QPushButton *m_deleteBtn;

    QList<Solution> m_solutions;
};