#pragma once

#include <QWidget>

class QPushButton;
class QVBoxLayout;

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

    void selectButton(int index);

signals:
    void itemSelected(int index);

private slots:
    void onButtonClicked(int index);

private:
    void setupUI();
    QPushButton *createButton(const QString &text, int index);

    QPushButton *m_snippetsBtn;
    QPushButton *m_solutionsBtn;
    QPushButton *m_searchBtn;
    QPushButton *m_stressTestBtn;
    QPushButton *m_workspacesBtn;
    QPushButton *m_templatesBtn;

    QPushButton *m_activeButton = nullptr;
};