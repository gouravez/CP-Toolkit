#pragma once

#include <QMainWindow>
#include <QStackedWidget>

class Sidebar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onSidebarItemSelected(int index);

private:
    void setupUi();
    void setupDatabase();

    Sidebar        *m_sidebar = nullptr;
    QStackedWidget *m_stack   = nullptr;
};