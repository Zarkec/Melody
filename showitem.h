#ifndef SHOWITEM_H
#define SHOWITEM_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCache>
#include <QThread>

#include "music.h"
#include "playlist.h"

namespace Ui
{
    class ShowItem;
}

class ShowItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShowItem(QWidget* parent = nullptr);
    ~ShowItem();
    void initLocalShowItem(const Music& music);
    void initNetworkShowItem(const Music& music);
    void initTopListShowItem(const Music& music);
    void initPlayListShowItem(const Playlist& playlist);
    void initPlayListMusicShowItem(const Music& music);
    void setMusicId(QString musicId);
    void setMusicName(QString musicName);
    void setMusicAuthor(QString musicAuthor);
    void setMusicPic(QString musicPicUrl);
    void networkReplyFinish(QNetworkReply* reply, QLabel* label);
    void setImageFromUrl(const QString& url, QLabel* label);

    Music getMusic();

private:
    Ui::ShowItem* ui;
    Music m_music;
    Playlist m_playlist;
};

#endif // SHOWITEM_H
