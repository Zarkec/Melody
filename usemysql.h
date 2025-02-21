#ifndef USEMYSQL_H
#define USEMYSQL_H

#include <QtSql/QSqlDatabase>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>

#include "music.h"

class UseMySQL
{
public:
    UseMySQL();
    ~UseMySQL();

    static UseMySQL* instance(); //单例模式

    //void getMusicInfos(QStringList& musicNameList, QStringList& musicUrlList, QStringList& musicAuthorList, QStringList& musicPicUrlList); //获取音乐信息
    QList<Music> getMusicFromMysql();
	void insertMusicToMysql(const Music& music); //插入音乐信息
	void deleteMusicFromMysql(const Music& music); //删除音乐信息
    void updateMusicToMysql(const Music& music); //更新音乐信息

    //添加历史记录
    void insertHistoryToMysql(const QString& history); //插入历史记录


private:
    void connectMySQL(); //连接数据库
    void closeMySQL(); //关闭数据库

private:
    QSqlDatabase datebase; //数据库
    Music m_music;

};

#endif // USEMYSQL_H
