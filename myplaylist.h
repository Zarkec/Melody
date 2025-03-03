#ifndef MYPLAYLIST_H
#define MYPLAYLIST_H

#include <QMediaPlaylist>
#include <QObject>

class MyPlayList : public QMediaPlaylist
{
    Q_OBJECT
public:
    explicit MyPlayList(QObject *parent = nullptr);
};

#endif // MYPLAYLIST_H
