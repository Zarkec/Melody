#include "usesqlite.h"

UseSqlite::UseSqlite() {}

void UseSqlite::connectSqlite()
{
    datebase=QSqlDatabase::addDatabase("QSQLITE");
}
