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

#include "usemysql.h"

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
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

public slots:
    void readHttpReply();
    void setImageFromUrl(const QString& url, QLabel* label);
    void search(QString id);

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
    void on_pushButton_search_clicked();
    void initPlayer();
    void on_pushButton_Prev_clicked(); //上一曲
    void on_pushButton_Next_clicked();//下一曲

private:
    Ui::Widget* ui;
    QPoint mOffSet;//偏移值
    bool mbPressed;//鼠标按下标志（不分左右键）
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    QMediaPlaylist* m_LocalPlaylist;
    QMediaPlaylist* m_NetworkPlaylist;
    QString* ctime;//当前时长
    QString* ttime;//总时长
    QString musicName;//音乐名
    QString musicAuthor;//音乐作者
    QString musicPicUrl;//封面图片
    QString musicUrl;//音乐地址
    QStringList musicNameList;
    QStringList musicUrlList;
    QStringList musicAuthorList;
    QStringList musicPicUrlList;
    QString songID;
    QNetworkReply* reply;
    void initBottom(QString musicName, QString musicAuthor, QString musicPicUrl);
    void setBottomByIndex(int index);
    void parseSongsJsonData(QByteArray rawData);
    void useMysql();//mysql
};

#endif // WIDGET_H
