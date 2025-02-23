#ifndef USENETWORK_H
#define USENETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QString>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include "music.h"

class UseNetwork : public QObject
{
    Q_OBJECT

public:
    UseNetwork(QObject* parent = nullptr);
    QList<Music> parseSearchJsonData(QByteArray rawData);
    void parseOnlineUrl(qint64 musicId);
    void parseOnlineUrlForList(QList<Music>& musicList);
    void getLiricByMusicId(qint64 musicId);
    ~UseNetwork();

public slots:
    void searchOnline(const QString& search);
signals:
    void searchFinished(const QList<Music>& musicList); // 信号：搜索完成时发出
    // 信号：请求完成后发送在线 URL
    void onlineUrlReady(const QString& onlineUrl);
    void onlineUrlForListReady(const QList<Music>& musicList);
    void lyricsReady(const QString& lyrics); // 信号：歌词获取完成时发出

private slots:
    void readSearchReply();
    void readPicUrlReply(QNetworkReply* reply, Music* music);
    void readLiricReply();

private:
    QNetworkAccessManager* manager;
    QNetworkReply* reply;
    QList<Music> m_musicList;
    int m_pendingRequests;
};

#endif // USENETWORK_H
