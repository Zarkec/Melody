#ifndef MUSIC_H
#define MUSIC_H

#include <QObject>

class Music
{
public:
    Music():
        m_id(0), m_musicId(0), m_name(""), m_author(""), m_album(""), m_url(""), m_picurl(""), m_duration(0) {}

    // Getter methods
    int id()const
    {
        return m_id;
    }

    qint64 musicId() const
    {
        return m_musicId;
    }

    QString name() const
    {
        return m_name;
    }

    QString author() const
    {
        return m_author;
    }

    QString url() const
    {
        return m_url;
    }

    QString picurl() const
    {
        return m_picurl;
    }

    QString album() const
    {
        return m_album;
    }

    int duration() const
    {
        return m_duration;
    }

    // Setter methods
    void setId(int id)
    {
        m_id = id;
    }

    void setMusicId(qint64 musicId)
    {
        m_musicId = musicId;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

    void setAuthor(const QString& author)
    {
        m_author = author;
    }

    void setAlbum(const QString& album)
    {
        m_album = album;
    }

    void setUrl(const QString& url)
    {
        m_url = url;
    }

    void setPicurl(const QString& picurl)
    {
        m_picurl = picurl;
    }

    void setDuration(int duration)
    {
        m_duration = duration;
    }

private:
    int m_id;
    qint64 m_musicId;
    QString m_name;
    QString m_author;
    QString m_album;
    QString m_url;//播放地址
    QString m_picurl;//歌曲图片地址
    int m_duration; // 单位：秒

};

#endif // MUSIC_H
