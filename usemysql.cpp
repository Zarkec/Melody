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
QList<Music> UseMySQL::getMusicFromMysql()
{
    if (!datebase.isOpen()) {
        connectMySQL();
    }
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

//插入音乐信息
void UseMySQL::insertMusicToMysql(const Music& music)
{
    if (!datebase.isOpen()) {
        connectMySQL();
    }
	QSqlQuery query(datebase);
	query.prepare("INSERT INTO music (music_name, music_url, music_author, music_picurl,album) VALUES (:music_name, :music_url, :music_author, :music_picurl ,:album);");
	query.bindValue(":music_name", music.name());
	query.bindValue(":music_url", music.url());
	query.bindValue(":music_author", music.author());
	query.bindValue(":music_picurl", music.picurl());
	query.bindValue(":album", music.album());

	if (query.exec())
	{
		qDebug() << "Insert succeed!";
	}
	else
	{
		qDebug() << "Insert failed!";
	}
}

void UseMySQL::deleteMusicFromMysql(const Music& music)
{
    if (!datebase.isOpen()) {
        connectMySQL();
    }
    if (music.id() == 0) {
        return;
    }
	QSqlQuery query(datebase);
    qDebug() << "Music MusicID:" << music.id();
    qDebug() << "Music Name:" << music.name();
    qDebug() << "Music Author:" << music.author();
    qDebug() << "Music Album:" << music.album();
    qDebug() << "Music Url:" << music.url();
    qDebug() << "Music Pic:" << music.picurl();
    qDebug() << "Music Duration:" << music.duration();
	query.prepare("DELETE FROM music WHERE music_id = :music_id;");
    query.bindValue(":music_id", music.id());
	if (query.exec())
	{
		qDebug() << "Delete succeed!";
	}
	else
	{
		qDebug() << "Delete failed!";
	}
}

void UseMySQL::updateMusicToMysql(const Music& music)
{
    if (!datebase.isOpen()) {
        connectMySQL();
    }
	QSqlQuery query(datebase);
	query.prepare("UPDATE music SET music_name = :music_name, music_url = :music_url, music_author = :music_author, music_picurl = :music_picurl,album = :album WHERE music_id = :music_id;");
	query.bindValue(":music_id", music.id());
	query.bindValue(":music_name", music.name());
	query.bindValue(":music_url", music.url());
	query.bindValue(":music_author", music.author());
	query.bindValue(":music_picurl", music.picurl());
	query.bindValue(":album", music.album());

	if (query.exec())
	{
		qDebug() << "Update succeed!";
	}
	else
	{
		qDebug() << "Update failed!";
	}
}

//连接数据库
void UseMySQL::connectMySQL() {
    if (!datebase.isOpen()) {
        datebase = QSqlDatabase::addDatabase("QMYSQL", "main");
        datebase.setDatabaseName("melody_db"); //设置需要连接的数据库名
        datebase.setHostName("localhost"); //设置本地主机
        datebase.setUserName("root"); //设置用户名
        datebase.setPassword("1234"); //设置密码
        if (!datebase.open()) {
            qDebug() << "Failed to connect to database!";
        }
        else {
            qDebug() << "Connect succeed!";
        }
    }
}


void UseMySQL::closeMySQL() {
    if (datebase.isOpen()) {
        datebase.close();
        QSqlDatabase::removeDatabase("main");
    }
}
