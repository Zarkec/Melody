#include "usemysql.h"

Q_GLOBAL_STATIC(UseMySQL, theInstance);

UseMySQL::UseMySQL()
{
    connectMySQL(); //连接数据库
}

UseMySQL::~UseMySQL()
{

}

UseMySQL* UseMySQL::instance()
{
    return theInstance;
}

//获取音乐信息
//void UseMySQL::getMusicInfos(QStringList& musicNameList, QStringList& musicUrlList, QStringList& musicAuthorList, QStringList& musicPicUrlList)
//{
// Music music;
// QSqlQueryModel sql;
// sql.setQuery("SELECT * FROM music;", datebase);

// int row = sql.rowCount();

// if(row <= 0)
// {
// qDebug() << "数据库没有记录";
// return;
// }

// qDebug() << row;

// QString musicName, musicUrl, musicAuthor, musicPicUrl;

// // 遍历每一行
// for (int i = 0; i < row; ++i)
// {
// QSqlRecord record = sql.record(i);
// music.setId(record.value("music_id").toInt());
// music.setName(record.value("music_name").toString());
// music.setPicurl(record.value("music_picurl").toString());
// music.setAuthor(record.value("music_author").toString());

// musicName = record.value("music_name").toString();
// musicUrl = record.value("music_url").toString();
// musicAuthor = record.value("music_author").toString();
// musicPicUrl = record.value("music_picurl").toString();
// //qDebug() << "musicName:" << musicName << "musicUrl:" << musicUrl;
// musicNameList << musicName;
// musicUrlList << musicUrl;
// musicAuthorList << musicAuthor;
// musicPicUrlList << musicPicUrl;
// }
//}

QList<Music> UseMySQL::getMusicFromMysql()
{
    QList<Music> musicList;
    Music music;
    QSqlQueryModel sql;
    sql.setQuery("SELECT * FROM music;", datebase);

    int row = sql.rowCount();

    if(row <= 0)
    {
        qDebug() << "There is no record in the database";
        return QList<Music> {};
    }

    qDebug() << row;

    //QString musicName, musicUrl, musicAuthor, musicPicUrl;

    // 遍历每一行
    for (int i = 0; i < row; ++i)
    {
        QSqlRecord record = sql.record(i);
        music.setId(record.value("music_id").toInt());
        music.setName(record.value("music_name").toString());
        music.setUrl(record.value("music_url").toString());
        music.setPicurl(record.value("music_picurl").toString());
        music.setAuthor(record.value("music_author").toString());
        musicList.append(music);
    }

    return musicList;
}

//连接数据库
void UseMySQL::connectMySQL()
{
    datebase = QSqlDatabase::addDatabase("QMYSQL", "main");
    datebase.setDatabaseName("melody_db"); //设置需要连接的数据库名
    datebase.setHostName("localhost"); //设置本地主机
    datebase.setUserName("root"); //设置用户名
    datebase.setPassword("1234"); //设置密码

    if(datebase.open())
    {
        qDebug() << "Connect succeed!";
    }
    else
    {
        qDebug() << "Connect failed!";
    }
}
