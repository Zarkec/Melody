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
    //setWindowFlags(Qt::FramelessWindowHint);//无边框

    mbPressed = false;

    player = new QMediaPlayer(this);
    m_LocalPlaylist = new QMediaPlaylist(this);
    m_NetworkPlaylist = new QMediaPlaylist(this);

    ui->radioButton_music->setChecked(true);
    ui->radioButton_playlist->setChecked(false);

    useMysql();
    initPlayer();
    switchPage();

	ShowTableItem* showTableItem = new ShowTableItem();
	ShowTableItem* showTableItem2 = new ShowTableItem();

    ui->tableWidget_recommend->clear();
	ui->tableWidget_recommend->verticalHeader()->setVisible(false);
	ui->tableWidget_recommend->horizontalHeader()->setVisible(false);
	ui->tableWidget_recommend->setShowGrid(false);
    ui->tableWidget_recommend->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_recommend->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_recommend->setSelectionMode(QAbstractItemView::NoSelection);
	//ui->tableWidget_recommend->setColumnCount(2);
	//ui->tableWidget_recommend->setRowCount(1);
    
    updateRecommendList();

	//ui->tableWidget_recommend->setCellWidget(0, 0, showTableItem);
	//ui->tableWidget_recommend->setCellWidget(0, 1, showTableItem2);
	////ui->tableWidget_recommend->setCellWidget(0, 2, showTableItem);
 //   ui->tableWidget_recommend->resizeColumnsToContents();
 //   ui->tableWidget_recommend->resizeRowsToContents();

    connect(ui->listWidgetLocal, &ListWidgetLocal::updateListWidgetLocal, this, &Widget::updateLocalMusicList);
    //ShowItem* showItem = new ShowItem();
    //QListWidgetItem* item = new QListWidgetItem(ui->listWidget_onlineSearch);

    //// 设置 QListWidgetItem 的大小
    //item->setSizeHint(showItem->sizeHint());

    //showItem->initNetworkShowItem(music);

    //// 将 ShowItem 添加到 QListWidget
    //ui->listWidget_onlineSearch->addItem(item);
    //ui->listWidget_onlineSearch->setItemWidget(item, showItem);

}

Widget::~Widget()
{
    delete ui;
}

void printMusicList(const QList<Music>& musicList)
{
    // 输出解析结果
    qDebug() << "====Parsed Music List:====";

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
}

void Widget::initPlayer()
{
    //初始化音量
    player->setVolume(ui->volumeSlider->value());
    connect(player, &QMediaPlayer::stateChanged, this, &Widget::do_stateChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &Widget::do_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &Widget::do_durationChanged);
    connect(player, &QMediaPlayer::currentMediaChanged, this, &Widget::do_currentMediaChanged);
	connect(ui->lineEditSearch, &QLineEdit::returnPressed, this, &Widget::on_pushButton_search_clicked);
    // 连接 stateChanged 信号到槽函数
    connect(player, &QMediaPlayer::stateChanged, this, &Widget::updatePlayButtonIcon);
    if (ui->listWidget_onlineSearch->count() == 0)
    {
        //UseNetwork* usenetwork = new UseNetwork(this); // 使用堆内存分配
        //// 连接信号和槽
        //Playlist tempPlaylist; // 创建一个自动变量而不是引用
        //tempPlaylist.setPlaylistId(3778678);
        //usenetwork->parseOnlinePlatListUrl(tempPlaylist);
        //connect(usenetwork, &UseNetwork::searchOnlinePlatListFinished, this, &Widget::updateListWidget);
        QThread* thread = new QThread(this);
        UseNetwork* worker = new UseNetwork(thread);
        Playlist tempPlaylist; // 创建一个自动变量而不是引用
        tempPlaylist.setPlaylistId(3778678);

        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, [worker, tempPlaylist]() {
            worker->parseOnlinePlatListUrl(tempPlaylist);
            });
        connect(worker, &UseNetwork::searchOnlinePlatListFinished, this, &Widget::updateListWidget);
        connect(worker, &UseNetwork::searchOnlinePlatListFinished, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(thread, &QThread::finished, worker, &QObject::deleteLater);

        thread->start();
        qDebug() << "Thread started:" << thread->isRunning();
    }
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
            //qDebug() << "鼠标全局位置：" << e->globalPos();
            //qDebug() << "鼠标相对位置：" << e->pos();
            //qDebug() << "窗口当前位置：" << this->pos();
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

//UI的更新
void Widget::updateUI(Music music)
{
    //更新底部信息
    QString musicName = music.name();
    QString musicAuthor = music.author();
    QString musicPicUrl = music.picurl();
    //设置音乐名
    ui->label_musicname->setText(musicName);
    //设置作者
    ui->label_author->setText(musicAuthor);
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

    //更新歌词
    UseNetwork* useNetwork = new UseNetwork(this);
    useNetwork->getLiricByMusicId(music.musicId());
    connect(useNetwork, &UseNetwork::lyricsReady, this, [=](QString lirics)
        {
            ui->lyricsListWidget->clear();
            for (QString liric : lirics.split("\n"))
            {
                //[00:07.77]Will you lay me down ? \r\n
                QString time = liric.mid(liric.indexOf("[") + 1, 8);
                //[00:07.77]

                liric = liric.mid(liric.indexOf("]") + 1);
                if (liric.isEmpty())
                {
                    continue;
                }
                //Will you lay me down ? \r\n
                ui->lyricsListWidget->addItem(liric);
            }
            if (lirics.isEmpty())
            {
                ui->lyricsListWidget->addItem("暂无歌词");
            }
            //ui->lyricsListWidget->addItem(liric);
        });
}

//mysql
void Widget::useMysql()
{
    UseMySQL* useMySQL = new UseMySQL();
    QList<Music> musicList = useMySQL->getMusicFromMysql();

    //输出解析结果
    printMusicList(musicList);

    ui->listWidgetLocal->clear(); // 清空列表

    for (const Music& music : musicList)
    {
        ShowItem* showItem = new ShowItem(ui->listWidgetLocal);
        QListWidgetItem* item = new QListWidgetItem(ui->listWidgetLocal);
        // 设置 QListWidgetItem 的大小
        item->setSizeHint(showItem->sizeHint());

        showItem->initLocalShowItem(music);
        // 将 ShowItem 添加到 QListWidget
        ui->listWidgetLocal->addItem(item);
        ui->listWidgetLocal->setItemWidget(item, showItem);

        // 将音乐 URL 添加到播放列表
        m_LocalPlaylist->addMedia(QUrl(music.url()));

    }

    //保存到localMuisicList
    m_localMusicList = musicList;

    //设置本地音乐列表窗体和本地音乐播放器列表默认选中下标为index
    int index = 0;
    m_LocalPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(m_LocalPlaylist);
    m_LocalPlaylist->setCurrentIndex(index);
}

void Widget::updateLocalMusicList()
{
    UseMySQL* useMySQL = new UseMySQL();
    QList<Music> musicList = useMySQL->getMusicFromMysql();
    ui->listWidgetLocal->clear(); // 清空列表

    for (const Music& music : musicList)
    {
        ShowItem* showItem = new ShowItem(ui->listWidgetLocal);
        QListWidgetItem* item = new QListWidgetItem(ui->listWidgetLocal);
        // 设置 QListWidgetItem 的大小
        item->setSizeHint(showItem->sizeHint());

        showItem->initLocalShowItem(music);
        // 将 ShowItem 添加到 QListWidget
        ui->listWidgetLocal->addItem(item);
        ui->listWidgetLocal->setItemWidget(item, showItem);

        // 将音乐 URL 添加到播放列表
        m_LocalPlaylist->addMedia(QUrl(music.url()));

    }

    //保存到localMuisicList
    m_localMusicList = musicList;

//    //设置本地音乐列表窗体和本地音乐播放器列表默认选中下标为index
//    //int index = 0;
//    m_LocalPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
//    player->setPlaylist(m_LocalPlaylist);
//    //m_LocalPlaylist->setCurrentIndex(index);

}

//页面的切换
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
        updateRecommendList();
    });

}

//更新搜索的item
void Widget::updateListWidget(const QList<Music>& musicList)
{
    ui->stackedWidget_search->setCurrentIndex(0);
    ui->listWidget_onlineSearch->clear();

    // 使用智能指针自动管理临时对象
    m_networkMusicList = musicList;

    // 先批量创建所有 QListWidgetItem（主线程必须操作）
    QList<QPair<QListWidgetItem*, ShowItem*>> items;
    for (const auto& music : musicList) {
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget_onlineSearch);
        ShowItem* showItem = new ShowItem(ui->listWidget_onlineSearch); // 指定父对象自动释放
        item->setSizeHint(showItem->sizeHint());
        items.append(qMakePair(item, showItem));
    }

    // 批量添加项（减少布局计算次数）
    ui->listWidget_onlineSearch->setUpdatesEnabled(false);
    for (const auto& pair : items) {
        ui->listWidget_onlineSearch->addItem(pair.first);
        ui->listWidget_onlineSearch->setItemWidget(pair.first, pair.second);
    }
    ui->listWidget_onlineSearch->setUpdatesEnabled(true);

    // 异步初始化内容（通过线程池）
    QtConcurrent::run([this, items, musicList]() {
        for (int i = 0; i < items.size(); ++i) {
            QMetaObject::invokeMethod(items[i].second, [=]() {
                items[i].second->initNetworkShowItem(musicList[i]);
                }, Qt::QueuedConnection);
        }
        });
}

//更新搜索的歌单
void Widget::updatePlayListWidget(const QList<Playlist>& playlist)
{
    ui->stackedWidget_search->setCurrentIndex(1); // 切换到播放列表页面
    ui->listWidget_onlinePlayListSearch->clear(); // 清空列表
    for (const Playlist& p : playlist)
    {
        qDebug() << "=========playlist==========";
        qDebug() << "Playlist ID:" << p.playlistId();
        qDebug() << "Playlist Name:" << p.name();
        qDebug() << "Playlist Pic URL:" << p.picurl();
    }

    for (const Playlist& p : playlist)
    {
        ShowPlayListItem* showPlayListItem = new ShowPlayListItem(ui->listWidget_onlinePlayListSearch);
        //ShowItem* showItem = new ShowItem();
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget_onlinePlayListSearch);

        // 设置 QListWidgetItem 的大小
        item->setSizeHint(showPlayListItem->sizeHint());

        showPlayListItem->initPlayListShowItem(p);

        // 将 ShowItem 添加到 QListWidget
        ui->listWidget_onlinePlayListSearch->addItem(item);
        ui->listWidget_onlinePlayListSearch->setItemWidget(item, showPlayListItem);
    }

}

void Widget::updatePlayListMuiscWidget(const QList<Music>& musicList)
{
    ui->listWidget_playlist_music->clear(); // 清空列表

    printMusicList(musicList);

    for (const Music& music : musicList)
    {
        ShowItem* showItem = new ShowItem(ui->listWidget_playlist_music);
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget_playlist_music);

        // 设置 QListWidgetItem 的大小
        item->setSizeHint(showItem->sizeHint());

        showItem->initPlayListMusicShowItem(music);

        // 将 ShowItem 添加到 QListWidget
        ui->listWidget_playlist_music->addItem(item);
        ui->listWidget_playlist_music->setItemWidget(item, showItem);
    }

    m_networkMusicList = musicList;

}

void Widget::updateNetworkMusicList(const QList<Music>& musicList)
{
    if (!m_networkMusicList.isEmpty()) {
        m_networkMusicList.clear();
        m_networkMusicList = musicList;
        m_NetworkPlaylist->clear();
    }

    printMusicList(musicList);

    for (const Music& music : musicList )
    {
        m_NetworkPlaylist->addMedia(QUrl(music.url()));
    }

    // 先设置单曲循环模式，然后再切换到顺序播放模式
    m_NetworkPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    player->setPlaylist(m_NetworkPlaylist);
    on_pushButton_bfmode_clicked();

    emit updateNetworkMusicListFinished();

}

void Widget::changeOnlineUrl(const QString& onlineUrl)
{

    qDebug() << "onlineurl:" << onlineUrl;
    player->setMedia(QUrl(onlineUrl));

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

//更新推荐列表
void Widget::updateRecommendList()
{
    UseMySQL* useMySQL = new UseMySQL();
    QList<Music> musicList = useMySQL->getMusicFromMysql();
    ui->tableWidget_recommend->clear(); // 清空列表

    printMusicList(musicList);


    ui->tableWidget_recommend->setRowCount((musicList.size() + 3) / 4); // 计算所需的行数
    ui->tableWidget_recommend->setColumnCount(4);

    for (int i = 0; i < musicList.size(); i++)
    {
        ShowTableItem* showTableItem = new ShowTableItem();
        showTableItem->initShowTableItem(&musicList[i]);
        ui->tableWidget_recommend->setCellWidget(i / 4, i % 4, showTableItem);
    }


    ui->tableWidget_recommend->resizeColumnsToContents();
    ui->tableWidget_recommend->resizeRowsToContents();
}

//关闭窗口
void Widget::on_pushButton_close_clicked()
{
    close();
}

// 最小化窗口
void Widget::on_pushButton_minimize_clicked()
{
    this->showMinimized();
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

    QMediaPlaylist* playList = player->playlist();
    int currentIndex = playList->currentIndex();

    if(playList == m_LocalPlaylist && currentIndex >= 0 && currentIndex < m_localMusicList.size())
    {
        Music tempMusic = m_localMusicList[currentIndex];
        updateUI(tempMusic);
    }
    else if(playList == m_NetworkPlaylist && currentIndex >= 0 && currentIndex < m_networkMusicList.size())
    {
        Music tempMusic = m_networkMusicList[currentIndex];
        updateUI(tempMusic);
    };

}

void Widget::do_currentMediaChanged(const QMediaContent& media)
{
    // 在这里处理当前媒体内容的变化
    qDebug() << "Current media changed to:" << media.canonicalUrl().toString();

    if(player->state() != QMediaPlayer::PlayingState)
    {
        on_pushButton_play_clicked();
    }
}

void Widget::updatePlayButtonIcon(QMediaPlayer::State state)
{
    switch (state)
    {
    case QMediaPlayer::StoppedState:
        ui->pushButton_play->setIcon(QIcon(":/res/img/play-circle-fill.png"));
        break;

    case QMediaPlayer::PlayingState:
        ui->pushButton_play->setIcon(QIcon(":/res/img/pause-circle-fill.png"));
        break;

    case QMediaPlayer::PausedState:
        ui->pushButton_play->setIcon(QIcon(":/res/img/play-circle-fill.png"));
        break;
    }
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
    //判断搜索模式
    //单曲(1)，歌手(100)，专辑(10)，歌单(1000)，用户(1002) 
    if(ui->radioButton_music->isChecked())
    {
        searchType = "1";
    }else if(ui->radioButton_playlist->isChecked())
    {
        searchType = "1000";
    }
    else
    {
        searchType = "1";
    }
    QString search = ui->lineEditSearch->text();
    qDebug() << "search:" << search;
    //存储搜索历史
    UseMySQL* useMySQL = new UseMySQL();
    useMySQL->insertHistoryToMysql(search);
    UseNetwork* usenetwork = new UseNetwork(this); // 使用堆内存分配
    // 连接信号和槽
    usenetwork->searchOnline(search, searchType);
    connect(usenetwork, &UseNetwork::searchMusicFinished, this, &Widget::updateListWidget);
    connect(usenetwork, &UseNetwork::searchPlayListFinished, this, &Widget::updatePlayListWidget);

}

//上一曲
void Widget::on_pushButton_Prev_clicked()
{
    QMediaPlaylist* tmp = player->playlist(); //获取当前播放器的音乐列表

    // 如果是单曲循环模式，切换到顺序播放模式
    if (tmp->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        on_pushButton_bfmode_clicked();
    }

    tmp->previous();

    player->play();
}

//下一曲
void Widget::on_pushButton_Next_clicked()
{
    QMediaPlaylist* tmp = player->playlist(); //获取当前播放器的音乐列表

    // 如果是单曲循环模式，切换到顺序播放模式
    if (tmp->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        on_pushButton_bfmode_clicked();
    }

    tmp->next();

    player->play();
}

void Widget::on_listWidget_onlineSearch_itemDoubleClicked(QListWidgetItem* item)
{
    ShowItem* showItem = qobject_cast<ShowItem*>(ui->listWidget_onlineSearch->itemWidget(item));
    Music music = showItem->getMusic();
    UseNetwork* useNetwork = new UseNetwork(this);

    QMediaPlaylist* playList = player->playlist();
    bool needUpdate = (playList != m_NetworkPlaylist) || m_networkMusicList[0].url().isEmpty();

    if (needUpdate)
    {
        player->stop();
        useNetwork->parseOnlineUrlForList(m_networkMusicList);
        disconnect(this, &Widget::updateNetworkMusicListFinished, nullptr, nullptr);
        connect(useNetwork, &UseNetwork::onlineUrlForListReady, this, &Widget::updateNetworkMusicList);
        connect(this, &Widget::updateNetworkMusicListFinished, this, [=]()
        {
            m_NetworkPlaylist->setCurrentIndex(music.id() - 1);
            player->play();
        });
    }
    else
    {
        m_NetworkPlaylist->setCurrentIndex(music.id() - 1);
        player->play();
    }

    updateUI(music);
}

void Widget::on_listWidgetLocal_itemDoubleClicked(QListWidgetItem* item)
{
    ShowItem* showItem = qobject_cast<ShowItem*>(ui->listWidgetLocal->itemWidget(item));
    Music music = showItem->getMusic();

    if (player->playlist() == m_NetworkPlaylist)
    {
        // 先设置单曲循环模式，然后再切换到顺序播放模式
        m_LocalPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        player->setPlaylist(m_LocalPlaylist);
        on_pushButton_bfmode_clicked();
    }

	//得到item的index
	int index = ui->listWidgetLocal->row(item);
    m_LocalPlaylist->setCurrentIndex(index);

}

// 切换播放模式
void Widget::on_pushButton_bfmode_clicked()
{
    QMediaPlaylist* currentPlaylist = player->playlist(); // 获取当前播放列表

    // 根据当前播放模式切换到下一个模式
    if (currentPlaylist->playbackMode() == QMediaPlaylist::Loop)
    {
        currentPlaylist->setPlaybackMode(QMediaPlaylist::Random);
        ui->pushButton_bfmode->setIcon(QIcon(":/res/img/random.png"));
    }
    else if (currentPlaylist->playbackMode() == QMediaPlaylist::Random)
    {
        currentPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->pushButton_bfmode->setIcon(QIcon(":/res/img/loopone.png"));
    }
    else if (currentPlaylist->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        currentPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
        ui->pushButton_bfmode->setIcon(QIcon(":/res/img/loop.png"));
    }

    qDebug() << "Current playback mode:" << currentPlaylist->playbackMode();
}

// 添加本地音乐按钮
void Widget::on_pushButton_add_clicked()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("添加音乐文件"), " ", tr("音乐文件(*.mp3)"));
	if (filePath.isEmpty())
	{
		return;
	}
	qDebug() << "filePath:" << filePath;

	QFileInfo fileInfo(filePath);
	QString fileName = fileInfo.fileName();
	qDebug() << "fileName:" << fileName;    
	Music music;
	music.setName(fileName);
	music.setUrl(filePath);
	music.setAuthor("未知");
	music.setAlbum("未知");
	music.setDuration(0);
	music.setPicurl(":/res/img/music.png");

	UseMySQL* useMySQL = new UseMySQL();
	useMySQL->insertMusicToMysql(music);

	//刷新本地音乐列表
    updateLocalMusicList();
}

void Widget::on_radioButton_music_toggled(bool checked)
{
    qDebug()<<"======================music"<<checked;
}


void Widget::on_radioButton_playlist_toggled(bool checked)
{
    qDebug()<<"======================playlist"<<checked;
}


void Widget::on_listWidget_onlinePlayListSearch_itemDoubleClicked(QListWidgetItem *item)
{
    ShowPlayListItem* showPlayListItem = qobject_cast<ShowPlayListItem*>(ui->listWidget_onlinePlayListSearch->itemWidget(item));
    Playlist playlist = showPlayListItem->getPlaylist();

    //UseNetwork* useNetwork = new UseNetwork(this);
    //useNetwork->parseOnlinePlatListUrl(playlist);
    //connect(useNetwork, &UseNetwork::searchOnlinePlatListFinished, this, &Widget::updatePlayListMuiscWidget);

    QThread* thread = new QThread(this);
    UseNetwork* worker = new UseNetwork(thread);

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, [worker, playlist]() {
        worker->parseOnlinePlatListUrl(playlist);
        });
    connect(worker, &UseNetwork::searchOnlinePlatListFinished, this, &Widget::updatePlayListMuiscWidget);
    connect(worker, &UseNetwork::searchOnlinePlatListFinished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);

    thread->start();
}

void Widget::on_listWidget_playlist_music_itemDoubleClicked(QListWidgetItem* item)
{
    ShowItem* showItem = qobject_cast<ShowItem*>(ui->listWidget_playlist_music->itemWidget(item));
    Music music = showItem->getMusic();
    UseNetwork* useNetwork = new UseNetwork(this);

    QMediaPlaylist* playList = player->playlist();
    bool needUpdate = (playList != m_NetworkPlaylist) || m_networkMusicList[0].url().isEmpty();

    if (needUpdate)
    {
        player->stop();
        useNetwork->parseOnlineUrlForList(m_networkMusicList);
        disconnect(this, &Widget::updateNetworkMusicListFinished, nullptr, nullptr);
        connect(useNetwork, &UseNetwork::onlineUrlForListReady, this, &Widget::updateNetworkMusicList);
        connect(this, &Widget::updateNetworkMusicListFinished, this, [=]()
            {
                m_NetworkPlaylist->setCurrentIndex(music.id() - 1);
                player->play();
            });
    }
    else
    {
        m_NetworkPlaylist->setCurrentIndex(music.id() - 1);
        player->play();
    }

    updateUI(music);

}

