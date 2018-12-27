#ifndef TESTDB_H
#define TESTDB_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class dbManager
{
public:
    dbManager();
   ~dbManager();
    QSqlDatabase *getDB();
    bool openDB();
private:
    QSqlDatabase db;
};

#endif // TESTDB_H
