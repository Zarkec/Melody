#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QFileDialog>

#include "usemysql.h"
#include "showitem.h"
#include "showplaylistitem.h"
#include "showtableitem.h"
#include "showplaylistmusicitem.h"
#include "usenetwork.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}

QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    void useMysql();//mysql
    void updateLocalMusicList();
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

public slots:
    void setImageFromUrl(const QString& url, QLabel* label);
private slots:
    //关闭窗口按键
    void on_pushButton_close_clicked();
    void do_stateChanged(QMediaPlayer::State state); //播放器状态发生变化
    void do_positionChanged(qint64 position); //播放位置发生变化
    void do_durationChanged(qint64 duration); //播放时长发生变化
    void do_currentMediaChanged(const QMediaContent& media);
    void updatePlayButtonIcon(QMediaPlayer::State state); //更新播放按钮图标
    void on_pushButton_play_clicked();
    void on_timeSlider_sliderMoved(int position);//进度条
    void on_volumeSlider_valueChanged(int value);//音量条
    void on_pushButton_search_clicked();//设置搜索键按下
    void initPlayer();
    void initTopPage();
    void updateTop(QListWidget* topList, int playListID);
    void on_pushButton_Prev_clicked(); //上一曲
    void on_pushButton_Next_clicked();//下一曲
    void on_listWidget_onlineSearch_itemDoubleClicked(QListWidgetItem* item);
    void on_listWidgetLocal_itemDoubleClicked(QListWidgetItem* item);
    void on_pushButton_bfmode_clicked();// 切换播放模式
    void on_pushButton_minimize_clicked();
    void on_pushButton_add_clicked();// 添加本地音乐按钮
    void on_radioButton_music_toggled(bool checked);
    void on_radioButton_playlist_toggled(bool checked);
    void on_listWidget_onlinePlayListSearch_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_playlist_music_itemDoubleClicked(QListWidgetItem *item);

public:
signals:
    void updateNetworkMusicListFinished();

private:
    Ui::Widget* ui;
    QPoint mOffSet;//偏移值
    bool mbPressed;//鼠标按下标志（不分左右键）
    QMediaPlayer* player;
    QMediaPlaylist* m_LocalPlaylist;
    QMediaPlaylist* m_NetworkPlaylist;
    QString* ctime;//当前时长
    QString* ttime;//总时长
    QString musicName;//音乐名
    QString musicAuthor;//音乐作者
    QString musicPicUrl;//封面图片
    QString musicUrl;//音乐地址
    QString searchType;
    QList<Music> m_localMusicList;
    QList<Music> m_networkMusicList;
    QString songID;
    QNetworkReply* reply;
    void updateUI(Music music);
    void switchPage();
    void updateListWidget(const QList<Music>& musicList, QListWidget* listWidget);
    void updateSearchListWidget(const QList<Music>& musicList);
    void updateListWidgetNew(const QList<Music>& musicList);
    void updateListWidgetHot(const QList<Music>& musicList);
    void updateListWidgetSoar(const QList<Music>& musicList);
    void updateNetworkMusicList(const QList<Music>& musicList);
    void updatePlayListWidget(const QList<Playlist>& playlist);
    void updatePlayListMuiscWidget(const QList<Music>& musicList);
    void updateRecommendList();
    void changeOnlineUrl(const QString& onlineUrl);
};

#endif // WIDGET_H
