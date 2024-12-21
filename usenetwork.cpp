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

void UseNetwork::searchOnline(const QString& search)
{
    QUrl url("http://music.163.com/api/search/get");
    QUrlQuery query;
    query.addQueryItem("s", search);
    query.addQueryItem("type", "1");
    query.addQueryItem("offset", "0");
    query.addQueryItem("limit", "60");
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UseNetwork::readSearchReply);
}

void UseNetwork::readSearchReply()
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
    m_musicList = parseSearchJsonData(rawData);

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

QList<Music> UseNetwork::parseSearchJsonData(QByteArray rawData)
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
            qDebug() << "Response:" << responseData;
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
