#ifndef MUSIC_H
#define MUSIC_H

#include <QObject>

class Music : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString author READ author WRITE setAuthor)
    Q_PROPERTY(QString album READ album WRITE setAlbum)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString picurl READ picurl WRITE setPicurl)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
public:
    explicit Music(QObject* parent = nullptr);

    // Getter methods
    int id() const
    {
        return m_id;
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

signals:

private:
    int m_id;
    QString m_name;
    QString m_author;
    QString m_album;
    QString m_url;//播放地址
    QString m_picurl;//歌曲图片地址
    int m_duration; // 单位：秒

};

#endif // MUSIC_H
