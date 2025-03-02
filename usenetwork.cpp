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
    query.addQueryItem("limit", "20");
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
    m_playlist = parsePlayListSearchJsonData(rawData);
    emit searchPlayListFinished(m_playlist);
}

// 官方的接口，获取歌单详情
void UseNetwork::readOnlinePlatListReply()
{
    QList<Music> playlist_musics;
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
    qDebug() << "================readOnlinePlatListReplyRawData===============" << rawData;
    playlist_musics = parsePlayListMusicsSearchJsonData(rawData);
    emit searchOnlinePlatListFinished(playlist_musics);
}

void UseNetwork::readOnlinePlatListReply2()
{
    QList<Music> playlist_musics;
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
    qDebug() << "================readOnlinePlatListReplyRawData===============" << rawData;
    playlist_musics = parsePlayListMusicsSearchJsonData2(rawData);
    emit searchOnlinePlatListFinished(playlist_musics);
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
                        music->setPicurl(albumObj["blurPicUrl"].toString());
                    }
                }
            }
        }
    }

    reply->deleteLater();
    m_pendingRequests--;

    if (m_pendingRequests == 0)
    {
        emit searchMusicFinished(m_musicList);
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

QList<Music> UseNetwork::parsePlayListMusicsSearchJsonData(QByteArray rawData)
{
    QList<Music> musicList;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    if (!jsonDoc.isNull() && jsonDoc.isObject())
    {
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.contains("result") && jsonObj["result"].isObject())
        {
            QJsonObject resultObj = jsonObj["result"].toObject();
            if (resultObj.contains("tracks") && resultObj["tracks"].isArray())
            {
                QJsonArray tracksArray = resultObj["tracks"].toArray();

                int counter = 1;
                for (const QJsonValue& trackValue : tracksArray)
                {
                    if (trackValue.isObject())
                    {
                        QJsonObject trackObj = trackValue.toObject();
                        Music music;
                        if (trackObj.contains("id") && trackObj["id"].isDouble())
                        {
                            qint64 MusicId = trackObj["id"].toVariant().toLongLong();
                            music.setMusicId(MusicId);
                            music.setId(counter);
                            counter++;
                        }
                        if (trackObj.contains("name") && trackObj["name"].isString())
                        {
                            music.setName(trackObj["name"].toString());
                        }
                        if (trackObj.contains("duration") && trackObj["duration"].isDouble())
                        {
                            music.setDuration(trackObj["duration"].toInt());
                        }
                        if (trackObj.contains("album") && trackObj["album"].isObject())
                        {
                            QJsonObject albumObj = trackObj["album"].toObject();
                            if (albumObj.contains("name") && albumObj["name"].isString())
                            {
                                music.setAlbum(albumObj["name"].toString());
                            }
                            if (albumObj.contains("blurPicUrl") && albumObj["blurPicUrl"].isString())
                            {
                                music.setPicurl(albumObj["blurPicUrl"].toString());
                            }
                        }
                        if (trackObj.contains("artists") && trackObj["artists"].isArray())
                        {
                            QJsonArray artistsArray = trackObj["artists"].toArray();
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

QList<Music> UseNetwork::parsePlayListMusicsSearchJsonData2(QByteArray rawData)
{
    QList<Music> musicList;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    if (!jsonDoc.isNull() && jsonDoc.isObject())
    {
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.contains("results") && jsonObj["results"].isArray())
        {
            QJsonArray resultArray = jsonObj["results"].toArray();
            if (!resultArray.isEmpty()) {
                QJsonObject resultObj = resultArray[0].toObject();
                if (resultObj.contains("List") && resultObj["List"].isArray())
                {
                    QJsonArray ListArray = resultObj["List"].toArray();

                    int counter = 1;
                    //只加载100首歌曲
                    if (ListArray.size() > 20) {
                          QJsonArray limitedListArray;
                          for (int i = 0; i < ListArray.size() && i < 20; ++i) {
                              limitedListArray.append(ListArray[i]);
                           }
                          ListArray = limitedListArray;
                    }
                    for (const QJsonValue& ListValue : ListArray)
                    {
                        if (ListValue.isObject())
                        {
                            QJsonObject ListObj = ListValue.toObject();
                            Music music;
                            if (ListObj.contains("rid") && ListObj["rid"].isDouble())
                            {
                                qint64 MusicId = ListObj["rid"].toVariant().toLongLong();
                                music.setMusicId(MusicId);
                                music.setId(counter);
                                counter++;
                            }
                            if (ListObj.contains("name") && ListObj["name"].isString())
                            {
                                music.setName(ListObj["name"].toString());
                            }
                            if (ListObj.contains("pic") && ListObj["pic"].isString())
                            {
                                //"pic": "https://p2.music.126.net/cmoE8PsdK_Yn9VJ8ZVCGrw==/109951170507596121.jpg?param=300y300"
                                QString originalUrl = ListObj["pic"].toString();
                                QString Url = originalUrl.left(originalUrl.indexOf('?'));
                                music.setPicurl(Url);
                            }
                            if (ListObj.contains("artist") && ListObj["artist"].isString())
                            {
                                music.setAuthor(ListObj["artist"].toString());
                            }
                            musicList.append(music);
                        }
                    }
                }
            }
        }
    }
    return musicList;
}

//QList<Music> UseNetwork::parsePlayListMusicsSearchJsonData2(QByteArray rawData) 
//{
//    QList<Music> musicList;
//    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
//    if (jsonDoc.isNull() || !jsonDoc.isObject()) 
//        return musicList;
//
//    QJsonObject jsonObj = jsonDoc.object();
//    if (!jsonObj.contains("results") || !jsonObj["results"].isArray())
//        return musicList;
//
//    QJsonArray resultArray = jsonObj["results"].toArray();
//    if (resultArray.isEmpty())
//        return musicList;
//
//    QJsonObject resultObj = resultArray[0].toObject();
//    if (!resultObj.contains("List") || !resultObj["List"].isArray())
//        return musicList;
//
//    QJsonArray listArray = resultObj["List"].toArray();
//
//    //只加载100首歌曲
//    // 只加载100首歌曲
//    //if (listArray.size() > 50) {
//    //    //listArray = listArray.mid(0, 100);
//
//    //    // 使用以下代码
//    //    QJsonArray limitedListArray;
//    //    for (int i = 0; i < listArray.size() && i < 50; ++i) {
//    //        limitedListArray.append(listArray[i]);
//    //    }
//    //    listArray = limitedListArray;
//    //}
//    // 预分配内存（减少动态扩容开销）
//    musicList.reserve(listArray.size());
//
//    // 并行处理核心逻辑
//    QFutureSynchronizer<void> synchronizer;
//    QMutex listMutex; // 用于线程安全操作musicList
//    std::atomic<int> counter(1); // 原子操作的计数器
//
//    QtConcurrent::blockingMap(listArray, [&](const QJsonValue& listValue) {
//        if (!listValue.isObject()) return;
//
//        QJsonObject listObj = listValue.toObject();
//        Music music;
//
//        // 原子操作保证计数唯一性
//        int currentId = counter.fetch_add(1, std::memory_order_relaxed);
//        music.setId(currentId);
//
//        if (listObj.contains("rid") && listObj["rid"].isDouble()) {
//            music.setMusicId(listObj["rid"].toVariant().toLongLong());
//        }
//        if (listObj.contains("name") && listObj["name"].isString()) {
//            music.setName(listObj["name"].toString());
//        }
//        if (listObj.contains("pic") && listObj["pic"].isString()) {
//            music.setPicurl(listObj["pic"].toString());
//        }
//        if (listObj.contains("artist") && listObj["artist"].isString()) {
//            music.setAuthor(listObj["artist"].toString());
//        }
//
//        // 线程安全的列表追加
//        QMutexLocker locker(&listMutex);
//        musicList.append(music);
//    });
//
//    return musicList;
//}

QList<Playlist> UseNetwork::parsePlayListSearchJsonData(QByteArray rawData)
{
    QList<Playlist> playlistList;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    if (!jsonDoc.isNull() && jsonDoc.isObject())
    {
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("result") && jsonObj["result"].isObject())
        {
            QJsonObject resultObj = jsonObj["result"].toObject();
            if (resultObj.contains("playlists") && resultObj["playlists"].isArray())
            {
                QJsonArray playlistsArray = resultObj["playlists"].toArray();

                int counter = 1;

                for (const QJsonValue& playlistValue : playlistsArray)
                {
                    if (playlistValue.isObject())
                    {
                        QJsonObject playlistObj = playlistValue.toObject();
                        Playlist playlist;

                        if (playlistObj.contains("id") && playlistObj["id"].isDouble())
                        {
                            qint64 playlistId = playlistObj["id"].toVariant().toLongLong();
                            playlist.setPlaylistId(playlistId);
                            playlist.setId(counter);
                            counter++;
                        }
                        if (playlistObj.contains("name") && playlistObj["name"].isString())
                        {
                            playlist.setName(playlistObj["name"].toString());
                        }
                        if (playlistObj.contains("coverImgUrl") && playlistObj["coverImgUrl"].isString())
                        {
                            playlist.setPicurl(playlistObj["coverImgUrl"].toString());
                        }

                        playlistList.append(playlist);
                    }
                }
            }
        }
    }
    return playlistList;
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

void UseNetwork::parseOnlinePlatListUrl(Playlist playlist)
{
    // 解析官方的url不稳定
    //QUrl url("https://music.163.com/api/playlist/detail?id=" + QString::number(playlist.playlistId()));
    //qDebug() << "===========playlist url" << url;
    //QNetworkRequest request(url);
    //QNetworkReply* reply = manager->get(request); // 这里定义 reply
    //connect(reply, &QNetworkReply::finished, this, &UseNetwork::readOnlinePlatListReply);
    QUrl url("https://musicbox-web-api.mu-jie.cc/wyylist/?id=" + QString::number(playlist.playlistId()));
    QNetworkRequest request(url);
    qDebug() << "===========playlist url" << url;
    QNetworkReply* reply = manager->get(request); // 这里定义 reply
    connect(reply, &QNetworkReply::finished, this, &UseNetwork::readOnlinePlatListReply2);
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
        else {
            qDebug() << "no lyric";
            emit lyricsReady("");
        }
    }

}
