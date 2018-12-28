#include "mainwindow.h"
#include <QApplication>
#include "dbManager.h"
#include "login.h"
#include "qdebug.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Login c;
    c.show();
    dbManager db;
    if(db.openDB()){
        qDebug()<<"scc";
    }
    else  {
        qDebug()<<"false";
    };

    return a.exec();
}
