#include "showplaylistmusicitem.h"
#include "ui_showplaylistmusicitem.h"

ShowPlayListMusicItem::ShowPlayListMusicItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShowPlayListMusicItem)
{
    ui->setupUi(this);
}

// 共享网络管理器
static QNetworkAccessManager& sharedManager() {
    static QNetworkAccessManager instance;
    return instance;
}

void ShowPlayListMusicItem::initPlayListMusicItem(const Music &music)
{
    //保存music类
    m_music = music;
    //设置id
    ui->label_musicid->setText(QString::number(music.id()));
    //设置歌名
    ui->label_musicname->setText(music.name());
    //设置歌手
    ui->label_author->setText(music.author());
    setMusicPic(music.picurl() + "?param=50y50");
}

void ShowPlayListMusicItem::setMusicPic(QString musicPicUrl)
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

void ShowPlayListMusicItem::networkReplyFinish(QNetworkReply* reply, QLabel* label)
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

void ShowPlayListMusicItem::setImageFromUrl(const QString& url, QLabel* label)
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

ShowPlayListMusicItem::~ShowPlayListMusicItem()
{
    delete ui;
}

Music ShowPlayListMusicItem::getMusic()
{
    return m_music;
}
