#include "mainwindow.h"

#include <QHBoxLayout>
#include <QWidget>

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

    m_stack = new QStackedWidget(this);
    layout->addWidget(m_stack, 1);
}

void MainWindow::setupDatabase()
{
    // TODO: initialize db and repositories
}