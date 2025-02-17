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
#include "showtableitem.h"
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
    void on_pushButton_play_clicked();
    void on_timeSlider_sliderMoved(int position);//进度条
    void on_volumeSlider_valueChanged(int value);//音量条
    void on_pushButton_search_clicked();//设置搜索键按下
    void initPlayer();
    void on_pushButton_Prev_clicked(); //上一曲
    void on_pushButton_Next_clicked();//下一曲

    void on_listWidget_onlineSearch_itemDoubleClicked(QListWidgetItem* item);

    void on_listWidgetLocal_itemDoubleClicked(QListWidgetItem* item);

    void on_pushButton_bfmode_clicked();// 切换播放模式

    void on_pushButton_minimize_clicked();

    void on_pushButton_add_clicked();// 添加本地音乐按钮

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
    QList<Music> m_localMusicList;
    QList<Music> m_networkMusicList;
    QString songID;
    QNetworkReply* reply;
    void initBottom(QString musicName, QString musicAuthor, QString musicPicUrl);
    void switchPage();
    void updateListWidget(const QList<Music>& musicList);
    void updateNetworkMusicList(const QList<Music>& musicList);
    void changeOnlineUrl(const QString& onlineUrl);
};

#endif // WIDGET_H
