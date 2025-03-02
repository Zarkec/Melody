#ifndef SHOWPLAYLISTMUSICITEM_H
#define SHOWPLAYLISTMUSICITEM_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCache>
#include <QThread>

#include "music.h"

namespace Ui {
class ShowPlayListMusicItem;
}

class ShowPlayListMusicItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShowPlayListMusicItem(QWidget *parent = nullptr);
    void initPlayListMusicItem(const Music& music);
    void setMusicPic(QString musicPicUrl);
    void setImageFromUrl(const QString& url, QLabel* label);
    void networkReplyFinish(QNetworkReply* reply, QLabel* label);
    ~ShowPlayListMusicItem();

    Music getMusic();

private:
    Ui::ShowPlayListMusicItem *ui;
    Music m_music;
};

#endif // SHOWPLAYLISTMUSICITEM_H
