#include "testdb.h"

testdb::testdb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); //database driver
    db.setHostName("192.168.64.132");  //database ip address
    db.setUserName("Doctor");   //database username
    db.setPassword("123456");   //database password
    db.setDatabaseName("Hospital_MIS");     //database table name
    if (db.open())
    {
        qDebug() << "succ";
    }
    else
    {
        qDebug()<<db.lastError();
        qDebug() << "failed";
        exit(0);
    }
}
