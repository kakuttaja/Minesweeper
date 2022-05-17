#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // App style sheet
    QFile styleFile(":/dark.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleFile.readAll());
    a.setStyleSheet(styleSheet);

    a.setWindowIcon(QIcon(":/icons/icons/flag.ico"));

    MainWindow w;
    w.show();
    return a.exec();
}
