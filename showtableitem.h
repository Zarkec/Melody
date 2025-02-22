#ifndef SHOWTABLEITEM_H
#define SHOWTABLEITEM_H

#include <QWidget>
#include <QDebug>
#include <QPainter>

#include "music.h"

namespace Ui {
class ShowTableItem;
}

class ShowTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShowTableItem(QWidget *parent = nullptr);
    ~ShowTableItem();
    void initShowTableItem(Music* music);
signals:
    void playMusic(Music* music);

private slots:
    void on_pushButton_play_clicked(Music* music);

private:
    Ui::ShowTableItem *ui;
    Music *m_music;
};

#endif // SHOWTABLEITEM_H
