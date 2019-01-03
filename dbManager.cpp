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

//用户注销时更新值班记录
void dbManager::updateDuty(int loginAccount)
{
    QDate curDate = QDate::currentDate();
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString curDT = curDateTime.toString("yyyy-MM-dd hh:mm:ss dddd");
    QStringList dutyIDList;
    QSqlQuery query;
    if(query.exec("select * from dutyrecord where staff_id="+QString::number(loginAccount)+";")){
        while(query.next()){
            QDate dutyRecordDate = query.value("real_start_date").toDate();
            if(dutyRecordDate.isValid()&&curDate==dutyRecordDate){
                dutyIDList.append(query.value("dutyrecord_id").toString());
            }
        }
    }
    else{
        qDebug()<<"没有登陆信息";
    }
    //更新时间
    for(int i=0;i<dutyIDList.size();i++){
        if(query.exec("update dutyrecord set real_end_date="+curDT+",status=1 where dutyrecord_id="+dutyIDList.at(i)+";")){
            if(query.numRowsAffected()>0){
                qDebug()<<"更新成功";
            }
        }
        else{
            qDebug()<<"没有登陆信息";
        }
    }
}
