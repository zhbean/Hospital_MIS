#ifndef TESTDB_H
#define TESTDB_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include "QSqlQuery"

class dbManager
{
public:
    dbManager();
   ~dbManager();
    QSqlDatabase *getDB();
    bool openDB();
    QSqlQuery selectDB(QString sql);
private:
    QSqlDatabase db;
};

#endif // TESTDB_H
