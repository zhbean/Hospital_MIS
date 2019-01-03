#ifndef TESTDB_H
#define TESTDB_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include "QSqlQuery"
#include "QDate"

class dbManager
{
public:
    dbManager();
   ~dbManager();
    QSqlDatabase *getDB();
    bool openDB();
    QSqlQuery selectDB(QString sql);
    QStringList getStaffAndRoom(int account,int dpmdetail);
    void updateDuty(int loginAccount);
private:
    QSqlDatabase db;
};

#endif // TESTDB_H
