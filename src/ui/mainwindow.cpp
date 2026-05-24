#include "mainwindow.h"
#include "sidebar.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CPT — Competitive Programming Toolkit");
    setMinimumSize(1100, 700);
    setupUi();
}

void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_sidebar = new Sidebar(this);
    m_stack   = new QStackedWidget(this);

    for (int i = 0; i < 6; i++)
        m_stack->addWidget(new QLabel(QString("Page %1").arg(i), this));

    connect(m_sidebar, &Sidebar::itemSelected,
            this,      &MainWindow::onSidebarItemSelected);

    layout->addWidget(m_sidebar);
    layout->addWidget(m_stack, 1);
}

void MainWindow::setupDatabase()
{
    // TODO: initialize db and repositories
}

void MainWindow::onSidebarItemSelected(int index)
{
    if (index >= 0 && index < m_stack->count())
        m_stack->setCurrentIndex(index);
}