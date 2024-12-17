#include "usenetwork.h"

UseNetwork::UseNetwork(QObject* parent)
    : QObject(parent), manager(nullptr), reply(nullptr)
{

}

void UseNetwork::searchOnline(QString& search)
{
    manager = new QNetworkAccessManager(this);
    QUrl url("http://music.163.com/api/search/get?s=" + search + "&type=1&offset=0&limit=15");
    qDebug() << url;
    QNetworkRequest res(url);
    reply = manager->get(res);
    //进行数据读取
    connect(reply, &QNetworkReply::finished, this, &UseNetwork::readHttpReply);

}

Music UseNetwork::parseSearchJsonData(QByteArray rawData)
{
    Music music;

    //将QByteArray 转换成QJsonDocument
    QJsonDocument jsonObj = QJsonDocument::fromJson(rawData);

    if(jsonObj.isNull() || !jsonObj.isObject())
    {
        qDebug() << "Failed to parse JSON data.";
        return music;
    }

}

void UseNetwork::readHttpReply()
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QString response = reply->readAll();
        qDebug() << "Response:" << response;
        QByteArray data = reply->readAll();
        parseSearchJsonData(data);
    }
    else
    {
        qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater(); // 释放 reply 对象
    reply = nullptr;
}

UseNetwork::~UseNetwork()
{

}
