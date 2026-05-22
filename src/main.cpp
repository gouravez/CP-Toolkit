#include <QApplication>
#include <QFile>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("CPT");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("CPT");

    QFile themeFile(":/themes/dark.qss");
    if (themeFile.open(QFile::ReadOnly | QFile::Text))
        app.setStyleSheet(QString::fromUtf8(themeFile.readAll()));

    MainWindow window;
    window.show();

    return app.exec();
}