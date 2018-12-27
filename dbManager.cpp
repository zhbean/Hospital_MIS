#include "dbManager.h"

dbManager::dbManager()
{
    db = QSqlDatabase::addDatabase("QMYSQL"); //database driver
    db.setHostName("192.168.1.56");  //database ip address
    db.setUserName("Doctor");   //database username
    db.setPassword("123456");   //database password
    db.setDatabaseName("Hospital_MIS");     //database table name
}
dbManager::~dbManager()
{
    db.close();
}
bool dbManager::openDB(){
    if (db.open())
    {
        return true;
    }
    else
    {
       return false;
    }
}
QSqlDatabase* dbManager::getDB(){
    return &db;
}

