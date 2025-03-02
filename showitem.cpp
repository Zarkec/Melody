#include "showitem.h"
#include "ui_showitem.h"

ShowItem::ShowItem(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ShowItem)
{
    ui->setupUi(this);
}

// 共享网络管理器
static QNetworkAccessManager& sharedManager() {
    static QNetworkAccessManager instance;
    return instance;
}

ShowItem::~ShowItem()
{
    delete ui;
}

void ShowItem::initLocalShowItem(const Music& music)
{
    //保存music类
    m_music = music;
    //设置id
    ui->label_musicid->setText(QString::number(music.id()));
    //设置歌名
    ui->label_musicname->setText(music.name());
    //设置歌手
    ui->label_author->setText(music.author());
    //设置专辑
    ui->label_album->setText(music.album());
    //设置添加时间不显示
    ui->label_addtime->setVisible(false);
    //设置时长
    ui->label_duration->setText(QString("%1:%2").arg(music.duration() / 1000 / 60, 2, 10, QChar('0')).arg(music.duration() / 1000 % 60, 2, 10, QChar('0')));
    //设置时长不可见
    ui->label_duration->setVisible(false);
    setMusicPic(music.picurl());

}

void ShowItem::initNetworkShowItem(const Music& music)
{
    //保存music类
    m_music = music;
    //设置id
    ui->label_musicid->setText(QString ::number(music.id()));
    //设置歌名
    ui->label_musicname->setText(music.name());
    //设置歌手
    ui->label_author->setText(music.author());
    //设置专辑
    ui->label_album->setText(music.album());
    //设置添加时间不显示
    ui->label_addtime->setVisible(false);
    //设置时长
    ui->label_duration->setText(QString("%1:%2").arg(music.duration() / 1000 / 60, 2, 10, QChar('0')).arg(music.duration() / 1000 % 60, 2, 10, QChar('0')));
    setMusicPic(music.picurl());
}

void ShowItem::initPlayListMusicShowItem(const Music& music)
{
    //保存music类
    m_music = music;
    //设置id
    ui->label_musicid->setText(QString::number(music.id()));
    //设置歌名
    ui->label_musicname->setText(music.name());
    //设置歌手
    ui->label_author->setText(music.author());
    //设置专辑
    ui->label_album->setText(music.album());
    //设置添加时间不显示
    ui->label_addtime->setVisible(false);
    //设置时长
    ui->label_duration->setText(QString("%1:%2").arg(music.duration() / 1000 / 60, 2, 10, QChar('0')).arg(music.duration() / 1000 % 60, 2, 10, QChar('0')));
    ui->label_duration->setVisible(false);
    ui->label_pic->setVisible(false);
    //setMusicPic(music.picurl());
}

void ShowItem::initPlayListShowItem(const Playlist& playlist)
{
    m_playlist = playlist;
    ui->label_musicid->setText(QString::number(playlist.id()));
    ui->label_musicname->setText(playlist.name());
    ui->label_author->setVisible(false);
    setMusicPic(playlist.picurl());

}

void ShowItem::setMusicId(QString musicId)
{
    ui->label_musicid->setText(musicId);
}

void ShowItem::setMusicName(QString musicName)
{
    ui->label_musicname->setText(musicName);
}

void ShowItem::setMusicAuthor(QString musicAuthor)
{
    ui->label_author->setText(musicAuthor);
}

void ShowItem::setMusicPic(QString musicPicUrl)
{
    QPixmap pixmap;

    //如果是本地文件
    if(pixmap.load(musicPicUrl))
    {
        ui->label_pic->setPixmap(pixmap);
    }
    else
    {
        setImageFromUrl(musicPicUrl, ui->label_pic);
    }

}

void networkReplyFinish(QNetworkReply* reply, QLabel* label)
{
    if (reply->error())
    {
        // 处理错误
        qDebug() << "Network error:" << reply->errorString();
    }
    else
    {
        // 读取网络响应的数据
        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll());
        label->setPixmap(pixmap);
    }

    reply->deleteLater(); // 删除reply对象
}

void ShowItem::setImageFromUrl(const QString& url, QLabel* label)
{
    static QCache<QString, QPixmap> imageCache(1024 * 1024 * 1); // 50MB缓存

    if (imageCache.contains(url)) {
        label->setPixmap(*imageCache[url]);
        return;
    }

    QNetworkRequest request(url);
    QNetworkReply* reply = sharedManager().get(request);

    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QPixmap* pixmap = new QPixmap();
            pixmap->loadFromData(reply->readAll());
            imageCache.insert(url, pixmap); // 自动管理内存
            label->setPixmap(*pixmap);
        }
        reply->deleteLater();
        });
}

Music ShowItem::getMusic()
{
    return m_music;
}
