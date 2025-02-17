#ifndef USESQLITE_H
#define USESQLITE_H

#include <QtSql/QSqlDatabase>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>

class UseSqlite
{
public:
    UseSqlite();
    ~UseSqlite();

private:
    void connectSqlite();
    QSqlDatabase datebase;
};

#endif // USESQLITE_H
