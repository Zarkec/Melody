#ifndef SHOWPLAYLISTITEM_H
#define SHOWPLAYLISTITEM_H

#include <QLabel>
#include <QWidget>
#include <QString>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "playlist.h"

namespace Ui {
class ShowPlayListItem;
}

class ShowPlayListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShowPlayListItem(QWidget *parent = nullptr);
    void initPlayListShowItem(const Playlist& playlist);
    void setPlayListPic(QString musicPicUrl);
    void networkReplyFinish(QNetworkReply* reply, QLabel* label);
    void setImageFromUrl(const QString& url, QLabel* label);

    Playlist getPlaylist() const;

    ~ShowPlayListItem();

private:
    Ui::ShowPlayListItem *ui;
    Playlist m_playlist;
};

#endif // SHOWPLAYLISTITEM_H
