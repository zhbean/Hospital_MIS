#include "mainwindow.h"
#include <QApplication>
#include "dbManager.h"
#include "login.h"
#include "qdebug.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Login c;
    dbManager db;
    c.show();
    //w.show();

    return a.exec();
}
