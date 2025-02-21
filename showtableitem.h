#ifndef SHOWTABLEITEM_H
#define SHOWTABLEITEM_H

#include <QWidget>
#include <QDebug>

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

private:
    Ui::ShowTableItem *ui;
    Music *music;
};

#endif // SHOWTABLEITEM_H
