#ifndef USEMYSQL_H
#define USEMYSQL_H

#include <QtSql/QSqlDatabase>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>

class UseMySQL
{
public:
    UseMySQL();
    ~UseMySQL();

    static UseMySQL* instance(); //单例模式

    void getMusicInfos(QStringList& musicNameList, QStringList& musicUrlList, QStringList& musicAuthorList, QStringList& musicPicUrlList); //获取音乐信息

private:
    void connectMySQL(); //连接数据库

private:
    QSqlDatabase datebase; //数据库

};

#endif // USEMYSQL_H
