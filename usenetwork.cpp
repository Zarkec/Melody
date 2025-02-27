#include "usenetwork.h"
#include <QDebug>

UseNetwork::UseNetwork(QObject* parent)
    : QObject(parent), manager(new QNetworkAccessManager(this)), m_pendingRequests(0)
{
}

UseNetwork::~UseNetwork()
{
    if (manager)
    {
        manager->deleteLater();
    }

    // 确保所有回复都已删除
    while (!m_musicList.isEmpty())
    {
        m_musicList.pop_back();
    }
}

void UseNetwork::searchOnline(const QString& search,QString searchType)
{
    QUrl url("http://music.163.com/api/search/get");
    QUrlQuery query;
    query.addQueryItem("s", search);
    //单曲(1)，歌手(100)，专辑(10)，歌单(1000)，用户(1002) 
    query.addQueryItem("type", searchType);
    query.addQueryItem("offset", "0");
    query.addQueryItem("limit", "35");
    url.setQuery(query);

    qDebug() << "search url" << url;

    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);
    if (searchType == "1")
    {
        connect(reply, &QNetworkReply::finished, this, &UseNetwork::readMusicSearchReply);
    }
    else if (searchType == "1000")
    {
        qDebug() << "===============playlist search============";
        connect(reply, &QNetworkReply::finished, this, &UseNetwork::readPlayListSearchReply);
    }
}

void UseNetwork::readMusicSearchReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
    {
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray rawData = reply->readAll();
    m_musicList = parseMusicSearchJsonData(rawData);

    // 发起获取图片URL的请求
    m_pendingRequests = m_musicList.size();

    for (Music& music : m_musicList)
    {
        QUrl url(QString("http://music.163.com/api/song/detail?ids=[%1]").arg(music.musicId()));
        QNetworkRequest request(url);
        QNetworkReply* picReply = manager->get(request);
        connect(picReply, &QNetworkReply::finished, this, [this, picReply, &music]()
        {
            readPicUrlReply(picReply, &music);
        });
    }

    reply->deleteLater();
}

void UseNetwork::readPlayListSearchReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
    {
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray rawData = reply->readAll();
    qDebug() << "================rawData===============" << rawData;
    m_musicList = parsePlayListSearchJsonData(rawData);
}

void UseNetwork::readPicUrlReply(QNetworkReply* reply, Music* music)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error getting pic URL:" << reply->errorString();
    }
    else
    {
        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        if (!jsonDoc.isNull() && jsonDoc.isObject())
        {
            QJsonObject jsonObj = jsonDoc.object();

            if (jsonObj.contains("songs") && jsonObj["songs"].isArray())
            {
                QJsonArray songsArray = jsonObj["songs"].toArray();

                if (!songsArray.isEmpty())
                {
                    QJsonObject songObj = songsArray[0].toObject();

                    if (songObj.contains("album") && songObj["album"].isObject())
                    {
                        QJsonObject albumObj = songObj["album"].toObject();
                        //qDebug() << "blurPicUrl" << albumObj["blurPicUrl"].toString() + "?param=300y300";
                        //设置为小图300*300
                        music->setPicurl(albumObj["blurPicUrl"].toString() + "?param=300y300");
                    }
                }
            }
        }
    }

    reply->deleteLater();
    m_pendingRequests--;

    if (m_pendingRequests == 0)
    {
        emit searchFinished(m_musicList);
    }
}

QList<Music> UseNetwork::parseMusicSearchJsonData(QByteArray rawData)
{
    QList<Music> musicList;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);

    if (!jsonDoc.isNull() && jsonDoc.isObject())
    {
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("result") && jsonObj["result"].isObject())
        {
            QJsonObject resultObj = jsonObj["result"].toObject();

            if (resultObj.contains("songs") && resultObj["songs"].isArray())
            {
                QJsonArray songsArray = resultObj["songs"].toArray();

                int counter = 1;

                for (const QJsonValue& songValue : songsArray)
                {
                    if (songValue.isObject())
                    {
                        QJsonObject songObj = songValue.toObject();
                        Music music;

                        if (songObj.contains("id") && songObj["id"].isDouble())
                        {
                            qint64 MusicId = songObj["id"].toVariant().toLongLong();
                            music.setMusicId(MusicId);
                            music.setId(counter);
                            counter++;
                            //music.setUrl(QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(MusicId));
                        }

                        if (songObj.contains("name") && songObj["name"].isString())
                        {
                            music.setName(songObj["name"].toString());
                        }

                        if (songObj.contains("duration") && songObj["duration"].isDouble())
                        {
                            music.setDuration(songObj["duration"].toInt());
                        }

                        if (songObj.contains("album") && songObj["album"].isObject())
                        {
                            QJsonObject albumObj = songObj["album"].toObject();

                            if (albumObj.contains("name") && albumObj["name"].isString())
                            {
                                music.setAlbum(albumObj["name"].toString());
                            }
                        }

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

                        musicList.append(music);
                    }
                }
            }
        }
    }

    return musicList;
}

QList<Music> UseNetwork::parsePlayListSearchJsonData(QByteArray rawData)
{
    QList<Playlist> playlist;

}

void UseNetwork::parseOnlineUrl(qint64 musicId)
{
    QUrl url("https://musicbox-web-api.mu-jie.cc/wyy/mp3?rid=" + QString::number(musicId));
    QNetworkRequest request(url);
    qDebug() << "onlineurl" << url;

    QNetworkReply* reply = manager->get(request); // 这里定义 reply

    // 连接 finished 信号以处理响应
    connect(reply, &QNetworkReply::finished, [ = ]()
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
            //qDebug() << "Response:" << responseData;
            QString onlineUrl = QString::fromUtf8(responseData);
            // 发送信号，传递在线 URL
            emit onlineUrlReady(onlineUrl);
        }
        else
        {
            // 处理错误
            qDebug() << "Error:" << reply->errorString();
            // 发送信号，传递在线 URL
            emit onlineUrlReady(" ");
        }

        // 释放资源
        reply->deleteLater();
    });
}

void UseNetwork::parseOnlineUrlForList(QList<Music>& musicList)
{
    m_musicList = musicList;
    // 发起获取在线URL的请求
    m_pendingRequests = m_musicList.size();

    for (Music& music : m_musicList)
    {
        // 发起获取在线URL的请求
        QUrl onlineUrl("https://musicbox-web-api.mu-jie.cc/wyy/mp3?rid=" + QString::number(music.musicId()));
        QNetworkRequest onlineRequest(onlineUrl);
        QNetworkReply* onlineReply = manager->get(onlineRequest);
        connect(onlineReply, &QNetworkReply::finished, this, [this, onlineReply, &music]()
        {
            if (onlineReply->error() != QNetworkReply::NoError)
            {
                qDebug() << "Error getting online URL:" << onlineReply->errorString();
            }
            else
            {
                QByteArray data = onlineReply->readAll();
                QString onlineUrl = QString::fromUtf8(data);
                music.setUrl(onlineUrl); // 假设 Music 类中有 setOnlineUrl 方法
            }

            onlineReply->deleteLater();
            m_pendingRequests--;

            if (m_pendingRequests == 0)
            {
                emit onlineUrlForListReady(m_musicList);
            }
        });
    }
}

void UseNetwork::getLiricByMusicId(qint64 musicId)
{
    QUrl url("http://music.163.com/api/song/media");
    QUrlQuery query;
    query.addQueryItem("id", QString::number(musicId));
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UseNetwork::readLiricReply);
}

void UseNetwork::readLiricReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
    {
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray rawData = reply->readAll();
    qDebug() << "rawData" << rawData;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);

    if (!jsonDoc.isNull() && jsonDoc.isObject())
    {
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("lyric") && jsonObj["lyric"].isString())
        {
            QString liric = jsonObj["lyric"].toString();
            qDebug() << "lyric:" << liric;
            emit lyricsReady(liric);
        }
    }

}
