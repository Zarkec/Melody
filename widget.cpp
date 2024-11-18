#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("Melody");
    setWindowIcon(QIcon(":/res/icon.png"));
    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setWindowFlags(Qt::FramelessWindowHint);//无边框
    mbPressed = false;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::search(QString id)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, []()
    {
        qDebug() << "manger finish!";
    });

    QUrl url("http://music.163.com/api/song/detail?ids=[" + id + "]");
    qDebug() << url;
    QNetworkRequest res(url);
    reply = manager->get(res);
    //网络请求后进行数据读取
    connect(reply, &QNetworkReply::finished, this, &Widget::readHttpReply);
}

void Widget::initPlayer()
{
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    //未完成：可通过按键设置播放模式
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playlist);
    //初始化音量
    player->setVolume(ui->volumeSlider->value());
    connect(player, &QMediaPlayer::stateChanged, this, &Widget::do_stateChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &Widget::do_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &Widget::do_durationChanged);
    connect(player, &QMediaPlayer::currentMediaChanged, this, &Widget::do_currentMediaChanged);

    //根据songID获取播放链接
    QString url = QString("http://music.163.com/song/media/outer/url?id=") + songID + ".mp3";
    qDebug() << "songID为：" << songID;
    qDebug() << url;
    player->setMedia(QMediaContent(QUrl(url))); // 替换为实际的在线音乐URL

}

void Widget::mousePressEvent(QMouseEvent* e)
{
    //鼠标左键
    if(e->button() == Qt::LeftButton)
    {
        //设置状态栏可以点击移动
        if(e->pos().y() < 35)
        {
            mbPressed = true;
            qDebug() << "鼠标全局位置：" << e->globalPos();
            qDebug() << "鼠标相对位置：" << e->pos();
            qDebug() << "窗口当前位置：" << this->pos();
            mOffSet = e->globalPos() - this->pos();
        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent* e)
{
    if(mbPressed)
    {
        this->move(e->globalPos() - mOffSet);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent*)
{
    mbPressed = false;
}

//解析json数据
void Widget::parseSongsJsonData(QByteArray rawData)
{
    QJsonDocument jsonObj = QJsonDocument::fromJson(rawData);

    if(!jsonObj.isNull() && jsonObj.isObject())
    {
        QJsonObject objRoot = jsonObj.object();

        if(objRoot.contains("songs") && objRoot["songs"].isArray())
        {
            QJsonArray songsArray = objRoot["songs"].toArray();

            for (QJsonValue val : songsArray)
            {
                if(val.isObject())
                {
                    QJsonObject obj = val.toObject();
                    QString name = obj["name"].toString();
                    //动态获取音乐名称
                    ui->label_musicname->setText(name);
                    songID = QString::number( obj["id"].toVariant().toULongLong());

                    if(obj.contains("album") && obj["album"].isObject())
                    {
                        QJsonObject album = obj["album"].toObject();
                        QString picUrl = album["blurPicUrl"].toString();
                        setImageFromUrl(picUrl, ui->label_pic);
                    }

                    if(obj.contains("artists") && obj["artists"].isArray())
                    {
                        QJsonArray artistsArray = obj["artists"].toArray();

                        for (QJsonValue val : artistsArray )
                        {
                            QJsonObject obj = val.toObject();
                            ui->label_anthor->setText(obj["name"].toString());
                            qDebug() << obj["name"].toString();
                        }
                    }
                }
            }
        }
    }
}

void networkReplyFinished(QNetworkReply* reply, QLabel* label)
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

void Widget::setImageFromUrl(const QString& url, QLabel* label)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    // 连接信号和槽
    QObject::connect(manager, &QNetworkAccessManager::finished,
                     [ = ](QNetworkReply* reply)
    {
        networkReplyFinished(reply, label);
    });

    // 发起网络请求
    QNetworkRequest request(url);
    manager->get(request);
}

void Widget::readHttpReply()
{
    QByteArray data = reply->readAll();
    parseSongsJsonData(data);
    qDebug() << songID;
    //qDebug() << QString::fromUtf8(data);
    initPlayer();

}

void Widget::on_pushButton_close_clicked()
{
    close();
}

void Widget::do_stateChanged(QMediaPlayer::State state)
{
    qDebug() << state;

}

void Widget::do_positionChanged(qint64 position)
{
    ui->label_ctime->setText(QString("%1:%2").arg(position / 1000 / 60, 2, 10, QChar('0')).arg(position / 1000 % 60, 2, 10, QChar('0')));
    ui->timeSlider->setValue(position);
}

void Widget::do_durationChanged(qint64 duration)
{
    ui->label_ttime->setText(QString("%1:%2").arg(duration / 1000 / 60, 2, 10, QChar('0')).arg(duration / 1000 % 60, 2, 10, QChar('0')));
    //设置播放进度条范围
    ui->timeSlider->setRange(0, duration);

}

void Widget::do_currentMediaChanged(const QMediaContent& media)
{
    // 在这里处理当前媒体内容的变化
    qDebug() << "Current media changed to:" << media.canonicalUrl().toString();

    // 停止当前播放的媒体
}

void Widget::on_pushButton_play_clicked()
{

    switch (player->state())
    {
        case QMediaPlayer::StoppedState:
            player->play();
            ui->pushButton_play->setIcon(QIcon(":/res/pause-circle-fill.png"));
            break;

        case QMediaPlayer::PlayingState:
            player->pause();
            ui->pushButton_play->setIcon(QIcon(":/res/play-circle-fill.png"));
            break;

        case QMediaPlayer::PausedState:
            player->play();
            ui->pushButton_play->setIcon(QIcon(":/res/pause-circle-fill.png"));
            break;

    }

}

//设置滑块改变音乐进度
void Widget::on_timeSlider_sliderMoved(int position)
{
    player->setPosition(position);
}

//设置音量条改变音量
void Widget::on_volumeSlider_valueChanged(int value)
{
    player->setVolume(value);
}

//设置搜索键按下
void Widget::on_pushButton_search_clicked()
{
    QString songid = ui->lineEditSearch->text();
    search(songid);
}
