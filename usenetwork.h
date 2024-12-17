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
    void searchOnline(const QString& search);
    QList<Music> parseSearchJsonData(QByteArray rawData);
    QList<Music> parsePicUrl(QList<Music> musicList);
    ~UseNetwork();
signals:
    void searchFinished(const QList<Music>& musicList); // 信号：搜索完成时发出

public slots:
    void readHttpReply();

private:
    QNetworkAccessManager* manager;
    QNetworkReply* reply;
};

#endif // USENETWORK_H
