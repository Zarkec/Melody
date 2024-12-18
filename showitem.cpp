#include "showitem.h"
#include "ui_showitem.h"

ShowItem::ShowItem(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ShowItem)
{
    ui->setupUi(this);
}

ShowItem::~ShowItem()
{
    delete ui;
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
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    // 连接信号和槽
    QObject::connect(manager, &QNetworkAccessManager::finished,
                     [ = ](QNetworkReply* reply)
    {
        networkReplyFinish(reply, label);
    });

    // 发起网络请求
    QNetworkRequest request(url);
    manager->get(request);
}
