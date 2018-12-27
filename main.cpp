#include "mainwindow.h"
#include <QApplication>
#include "testdb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    testdb db;

    return a.exec();
}
