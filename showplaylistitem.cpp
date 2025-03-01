#include "showplaylistitem.h"
#include "ui_showplaylistitem.h"

ShowPlayListItem::ShowPlayListItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShowPlayListItem)
{
    ui->setupUi(this);
}

void ShowPlayListItem::initPlayListShowItem(const Playlist& playlist)
{
    m_playlist = playlist;
    ui->label_id->setText(QString::number(playlist.id()));
    ui->label_id->setVisible(false);
    ui->label_playlistname->setText(playlist.name());
    //ui->label_pic->setVisible(false);
    setPlayListPic(playlist.picurl());
}


void ShowPlayListItem::setPlayListPic(QString playlistPicUrl)
{
    QPixmap pixmap;

    //如果是本地文件
    if(pixmap.load(playlistPicUrl))
    {
        ui->label_pic->setPixmap(pixmap);
    }
    else
    {
        setImageFromUrl(playlistPicUrl, ui->label_pic);
    }

}

void ShowPlayListItem::networkReplyFinish(QNetworkReply* reply, QLabel* label)
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

void ShowPlayListItem::setImageFromUrl(const QString& url, QLabel* label)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    // 连接信号和槽
    QObject::connect(manager, &QNetworkAccessManager::finished,
        [=](QNetworkReply* reply)
        {
            networkReplyFinish(reply, label);
        });

    // 发起网络请求
    QNetworkRequest request(url);
    manager->get(request);
}

Playlist ShowPlayListItem::getPlaylist() const
{
    return m_playlist;
}

ShowPlayListItem::~ShowPlayListItem()
{
    delete ui;
}
