#pragma once
#include <QObject>
#include <QList>
#include "music.h"

class Playlist {
public:
    Playlist():m_id(0), m_playlistId(0), m_name(""), m_url(""), m_picurl("") {};
    ~Playlist() {};

    int id() const { return m_id; };
    qint64 playlistId() const { return m_playlistId; };
    QString name() const { return m_name; };
    QString url() const { return m_url; };
    QString picurl() const { return m_picurl; };
    QList<Music*> musics() const { return m_musics; };

    void setId(int id) { m_id = id; };
    void setPlaylistId(qint64 playlistId) { m_playlistId = playlistId; };
    void setName(QString name) { m_name = name; };
    void setUrl(QString url) { m_url = url; };
    void setPicurl(QString picurl) { m_picurl = picurl; };

private:
    int m_id;
    qint64 m_playlistId;
    QString m_name;
    QString m_url;
    QString m_picurl;
    QList<Music*> m_musics;
};