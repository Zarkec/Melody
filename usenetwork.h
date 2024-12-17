#ifndef USENETWORK_H
#define USENETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QString>
#include <QDebug>

class UseNetwork : public QObject
{
    Q_OBJECT

public:
    UseNetwork(QObject* parent = nullptr);
    void searchOnline(QString& search);
    ~UseNetwork();
public slots:
    void readHttpReply();

private:
    QNetworkAccessManager* manager;
    QNetworkReply* reply;
};

#endif // USENETWORK_H
