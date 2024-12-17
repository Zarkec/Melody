#include "usenetwork.h"

UseNetwork::UseNetwork(QObject* parent)
    : QObject(parent), manager(nullptr), reply(nullptr)
{

}

UseNetwork::~UseNetwork()
{

}

void UseNetwork::searchOnline(const QString& search)
{
    if (manager)
    {
        manager->deleteLater();
    }

    manager = new QNetworkAccessManager(this);
    QUrl url("http://music.163.com/api/search/get?s=" + search + "&type=1&offset=0&limit=20");
    qDebug() << "URL:" << url;
    QNetworkRequest res(url);
    reply = manager->get(res);
    //进行数据读取
    connect(reply, &QNetworkReply::finished, this, &UseNetwork::readHttpReply);

}

QList<Music> UseNetwork::parsePicUrl(QList<Music> musicList)
{
    Music music = musicList[0];

    manager = new QNetworkAccessManager(this);
    QUrl url("http://music.163.com/api/song/detail?ids=[" + QString::number(music.id()) + "]");
    qDebug() << "URL:" << url;
    QNetworkRequest res(url);
    QNetworkReply* reply = manager->get(res);
    //进行数据解析
    connect(reply, &QNetworkReply::finished, this, [ & ]()
    {
        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        if(!jsonDoc.isNull() && jsonDoc.isObject())
        {
            QJsonObject jsonObj = jsonDoc.object();

            if(jsonObj.contains("songs") && jsonObj["songs"].isArray())
            {
                QJsonArray songsArray = jsonObj["songs"].toArray();

                for(const QJsonValue& val : songsArray)
                {
                    if(val.isObject())
                    {
                        QJsonObject obj = val.toObject();

                        if(obj.contains("album") && obj["album"].isObject())
                        {
                            QJsonObject album = obj["album"].toObject();
                            music.setPicurl(album["blurPicUrl"].toString());
                            break;
                            qDebug() << "blurPicUrl" << album["blurPicUrl"].toString();
                        }

                    }
                }

            }
        }
        else
        {
            qDebug() << "Error parsing pic URL:" << reply->errorString();
        }

        reply->deleteLater();
    });

    return musicList;

}

QList<Music> UseNetwork::parseSearchJsonData(QByteArray rawData)
{
    QList<Music> musicList; // 创建一个 Music 对象的列表

    //将QByteArray 转换成QJsonDocument
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);

    if(jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Failed to parse JSON data.";
        return musicList; // 返回一个空的 Music 列表
    }

    //获取Json对象
    QJsonObject jsonObj = jsonDoc.object();

    //检查是否包含"result"字段
    if(jsonObj.contains("result") && jsonObj["result"].isObject())
    {
        QJsonObject resultObj = jsonObj["result"].toObject();

        //检查是否包含"songs"字段
        if(resultObj.contains("songs") && resultObj["songs"].isArray())
        {
            QJsonArray songsArray = resultObj["songs"].toArray();

            // 遍历所有歌曲
            for (const QJsonValue& songValue : songsArray)
            {
                if (!songValue.isObject())
                {
                    continue; // 跳过非对象的元素
                }

                QJsonObject songObj = songValue.toObject();
                Music music; // 创建一个 Music 对象

                // 提取歌曲信息
                if (songObj.contains("id") && songObj["id"].isDouble())
                {
                    //int id = songObj["id"].toInt();
                    //解决int溢出问题
                    qint64 id = songObj["id"].toVariant().toLongLong();
                    music.setId(id);
                    music.setUrl(QString("http://music.163.com/song/media/outer/url?id=") + QString::number(id) + ".mp3");
                }

                if (songObj.contains("name") && songObj["name"].isString())
                {
                    music.setName(songObj["name"].toString());
                }

                if (songObj.contains("duration") && songObj["duration"].isDouble())
                {
                    music.setDuration(songObj["duration"].toInt());
                }

                // 提取专辑信息
                if (songObj.contains("album") && songObj["album"].isObject())
                {
                    QJsonObject albumObj = songObj["album"].toObject();

                    if (albumObj.contains("name") && albumObj["name"].isString())
                    {
                        music.setAlbum(albumObj["name"].toString());
                    }
                }

                // 提取歌手信息
                if (songObj.contains("artists") && songObj["artists"].isArray())
                {
                    QJsonArray artistsArray = songObj["artists"].toArray();

                    if (!artistsArray.isEmpty())
                    {
                        QJsonObject artistObj = artistsArray[0].toObject();

                        if (artistObj.contains("name") && artistObj["name"].isString())
                        {
                            music.setAuthor(artistObj["name"].toString());
                        }
                    }
                }

                // 将填充好的 Music 对象添加到列表中
                musicList.append(music);
            }
        }

    }

    return musicList;
}

void UseNetwork::readHttpReply()
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray rawData = reply->readAll();
        QList<Music> musicList = parseSearchJsonData(rawData); // 解析 JSON 数据
        //QList<Music> musicListWithPicurl = parsePicUrl(musicList);

        // 发出信号，传递解析后的数据
        emit searchFinished(musicList);

        // 输出解析结果
        // qDebug() << "Parsed Music List:";

        // for (const Music& music : musicList)
        // {
        // qDebug() << "ID:" << music.id();
        // qDebug() << "Name:" << music.name();
        // qDebug() << "Author:" << music.author();
        // qDebug() << "Album:" << music.album();
        // qDebug() << "URL:" << music.url();
        // qDebug() << "Pic URL:" << music.picurl();
        // qDebug() << "Duration:" << music.duration();
        // qDebug() << "-----------------------------";
        // }
    }
    else
    {
        qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater(); // 释放 reply 对象
    reply = nullptr;
}
