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
QStringList dbManager::getStaffAndRoom(int account,int dpmdetail){
    QSqlQuery query;
    QStringList information;
    QString Staffsql="select * from staff where staff_id="+QString::number(account)+";";
    QString dpmdetailsql="select * from dpmdetail where dpmdetail_id="+QString::number(dpmdetail)+";";
    query.prepare(Staffsql);
    query.exec();
    while(query.next()){
        QString name=query.value("staff_name").toString();
        qDebug()<<name;
        information.insert(0,name);
    }
    query.prepare(dpmdetailsql);
    query.exec();
    while (query.next()) {
        QString dpmdetailName=query.value("dpmdetail_name").toString();
        information.insert(1,dpmdetailName);
    }
    return information;
}
