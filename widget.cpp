#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("Melody");
    setWindowIcon(QIcon(":/res/img/icon.png"));
    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setWindowFlags(Qt::FramelessWindowHint);//无边框
    mbPressed = false;

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    m_LocalPlaylist = new QMediaPlaylist(this);
    m_NetworkPlaylist = new QMediaPlaylist(this);

    //未完成：可通过按键设置播放模式
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playlist);

    useMysql();
    initPlayer();
    switchPage();
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
    //初始化音量
    player->setVolume(ui->volumeSlider->value());
    connect(player, &QMediaPlayer::stateChanged, this, &Widget::do_stateChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &Widget::do_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &Widget::do_durationChanged);
    connect(player, &QMediaPlayer::currentMediaChanged, this, &Widget::do_currentMediaChanged);

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
                    musicName = obj["name"].toString();
                    //动态获取音乐名称
                    //ui->label_musicname->setText(musicName);
                    songID = QString::number( obj["id"].toVariant().toULongLong());

                    if(obj.contains("album") && obj["album"].isObject())
                    {
                        QJsonObject album = obj["album"].toObject();
                        musicPicUrl = album["blurPicUrl"].toString();
                        //setImageFromUrl(musicPicUrl, ui->label_pic);
                    }

                    if(obj.contains("artists") && obj["artists"].isArray())
                    {
                        QJsonArray artistsArray = obj["artists"].toArray();

                        for (QJsonValue val : artistsArray )
                        {
                            QJsonObject obj = val.toObject();
                            musicAuthor = obj["name"].toString();
                            //ui->label_anthor->setText(musicAuthor);
                            qDebug() << obj["name"].toString();
                        }
                    }
                }
            }
        }
    }

    initBottom(musicName, musicAuthor, musicPicUrl);
}

//底部信息的更新
void Widget::initBottom(QString musicName, QString musicAuthor, QString musicPicUrl)
{
    //设置音乐名
    ui->label_musicname->setText(musicName);
    //设置作者
    ui->label_anthor->setText(musicAuthor);
    //设置音乐图片

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

//mysql
void Widget::useMysql()
{
    UseMySQL::instance()->getMusicInfos(musicNameList, musicUrlList, musicAuthorList, musicPicUrlList);
    qDebug() << musicNameList;

    // 检查数据是否正确读取
    qDebug() << "Music Names:" << musicNameList;
    qDebug() << "Music URLs:" << musicUrlList;
    qDebug() << "Music Authors:" << musicAuthorList;
    qDebug() << "Music Pic URLs:" << musicPicUrlList;

    //设置QListWidget
    //QListWidgetItem* item = new QListWidgetItem(ui->listWidgetLocal);

    // for (int i = 0; i < musicNameList.count(); i++)
    // {
    // //item = new QListWidgetItem(musicNameList[i], ui->listWidgetLocal);
    // m_LocalPlaylist->addMedia(QUrl(musicUrlList[i]));
    // }
    // ShowItem* showItem = new ShowItem();
    // item->setSizeHint(showItem->sizeHint());
    // ui->listWidgetLocal->addItem(item);
    // ui->listWidgetLocal->setItemWidget(item, showItem);

    QListWidgetItem* item = nullptr;
    ShowItem* showItem = nullptr;

    for (int i = 0; i < musicNameList.size(); i++ )
    {
        showItem = new ShowItem();
        item = new QListWidgetItem(ui->listWidgetLocal);
        item->setSizeHint(showItem->sizeHint());
        showItem->setMusicId(QString::number(i + 1));
        showItem->setMusicName(musicNameList[i]);
        showItem->setMusicAuthor(musicAuthorList[i]);
        showItem->setMusicPic(musicPicUrlList[i]);
        ui->listWidgetLocal->addItem(item);
        ui->listWidgetLocal->setItemWidget(item, showItem);
        m_LocalPlaylist->addMedia(QUrl(musicUrlList[i]));
    }

    //设置本地音乐列表窗体和本地音乐播放器列表默认选中下标为index
    int index = 1;
    m_LocalPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(m_LocalPlaylist);
    m_LocalPlaylist->setCurrentIndex(index);
    setBottomByIndex(index);
}

void Widget::switchPage()
{
    connect(ui->pushButton_pageSearch, &QPushButton::clicked, this, [ = ]()
    {
        ui->stackedWidget->setCurrentIndex(0);
    });

    connect(ui->pushButton_pageLocal, &QPushButton::clicked, this, [ = ]()
    {
        ui->stackedWidget->setCurrentIndex(1);
    });

    connect(ui->pushButton_pageRecommend, &QPushButton::clicked, this, [ = ]()
    {
        ui->stackedWidget->setCurrentIndex(2);
    });

}

//更新搜索的item
void Widget::updateListWidget(const QList<Music>& musicList)
{
    ui->listWidget_onlineSearch->clear(); // 清空列表
    // 输出解析结果
    qDebug() << "Parsed Music List:";

    for (const Music& music : musicList)
    {
        qDebug() << "ID:" << music.id();
        qDebug() << "Music ID:" << music.musicId();
        qDebug() << "Name:" << music.name();
        qDebug() << "Author:" << music.author();
        qDebug() << "Album:" << music.album();
        qDebug() << "URL:" << music.url();
        qDebug() << "Pic URL:" << music.picurl();
        qDebug() << "Duration:" << music.duration();
        qDebug() << "-----------------------------";
    }

    for (const Music& music : musicList)
    {
        ShowItem* showItem = new ShowItem();
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget_onlineSearch);

        // 设置 QListWidgetItem 的大小
        item->setSizeHint(showItem->sizeHint());

        showItem->initNetworkShowItem(music);

        // 设置 ShowItem 的内容
        // showItem->setMusicId(QString::number(music.id()));
        // showItem->setMusicName(music.name());
        // showItem->setMusicAuthor(music.author());
        // showItem->setMusicPic(music.picurl());

        // 将 ShowItem 添加到 QListWidget
        ui->listWidget_onlineSearch->addItem(item);
        ui->listWidget_onlineSearch->setItemWidget(item, showItem);

        // 将音乐 URL 添加到播放列表
        m_NetworkPlaylist->addMedia(QUrl(music.url()));
    }
}

void Widget::setBottomByIndex(int index)
{
    musicName = musicNameList[index];
    musicAuthor = musicAuthorList[index];
    musicPicUrl = musicPicUrlList[index];
    initBottom(musicName, musicAuthor, musicPicUrl);
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

    //根据songID获取播放链接
    QString url = QString("http://music.163.com/song/media/outer/url?id=") + songID + ".mp3";
    qDebug() << "songID为：" << songID;
    qDebug() << url;
    m_NetworkPlaylist->addMedia(QMediaContent(QUrl(url)));
    m_NetworkPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(m_NetworkPlaylist); // 替换为实际的在线音乐URL
}

//关闭窗口
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

//播放按键的变化
void Widget::on_pushButton_play_clicked()
{

    switch (player->state())
    {
        case QMediaPlayer::StoppedState:
            player->play();
            ui->pushButton_play->setIcon(QIcon(":/res/img/pause-circle-fill.png"));
            break;

        case QMediaPlayer::PlayingState:
            player->pause();
            ui->pushButton_play->setIcon(QIcon(":/res/img/play-circle-fill.png"));
            break;

        case QMediaPlayer::PausedState:
            player->play();
            ui->pushButton_play->setIcon(QIcon(":/res/img/pause-circle-fill.png"));
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
    //QString songid = ui->lineEditSearch->text();
    //search(songid);
    QString search = ui->lineEditSearch->text();
    UseNetwork* usenetwork = new UseNetwork(this); // 使用堆内存分配
    // 连接信号和槽
    connect(usenetwork, &UseNetwork::searchFinished, this, &Widget::updateListWidget);
    usenetwork->searchOnline(search);

}

//上一曲
void Widget::on_pushButton_Prev_clicked()
{
    QMediaPlaylist* tmp = player->playlist(); //获取当前播放器的音乐列表
    int curIndex = tmp->currentIndex(); //获取当前播放音乐的下标
    curIndex--;

    if (curIndex < 0)
    {
        curIndex = tmp->mediaCount() - 1;
    };

    tmp->setCurrentIndex(curIndex); //设置当前音乐播放器列表的下标

    setBottomByIndex(curIndex);
}

//下一曲
void Widget::on_pushButton_Next_clicked()
{
    QMediaPlaylist* tmp = player->playlist(); //获取当前播放器的音乐列表
    int curIndex = tmp->currentIndex(); //获取当前播放音乐的下标
    curIndex++;

    if (curIndex >= tmp->mediaCount())
    {
        curIndex = 0;
    };

    tmp->setCurrentIndex(curIndex); //设置当前音乐播放器列表的下标

    setBottomByIndex(curIndex);
}
