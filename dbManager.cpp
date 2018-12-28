#include "dbManager.h"

dbManager::dbManager()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QMYSQL"); //database driver
        db.setHostName("192.168.1.56");  //database ip address
        db.setUserName("Doctor");   //database username
        db.setPassword("123456");   //database password
        db.setDatabaseName("Hospital_MIS");     //database table name
    }
}
dbManager::~dbManager()
{
    db.close();
}
bool dbManager::openDB(){
    if (db.open())
    {
        qDebug()<<"succ";
        return true;
    }
    else
    {
        qDebug()<<"failed";
       return false;
    }
}
QSqlDatabase* dbManager::getDB(){
    return &db;
}

QSqlQuery dbManager::selectDB(QString sql){
    QSqlQuery query;
    query.prepare(sql);
    return query;
}

