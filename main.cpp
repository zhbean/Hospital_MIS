#include "mainwindow.h"
#include <QApplication>
#include "dbManager.h"
#include "qdebug.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    dbManager db;
    if(db.openDB()){
        qDebug()<<"scc";
    };

    return a.exec();
}
