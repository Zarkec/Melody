#ifndef SHOWTABLEITEM_H
#define SHOWTABLEITEM_H

#include <QWidget>

namespace Ui {
class ShowTableItem;
}

class ShowTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShowTableItem(QWidget *parent = nullptr);
    ~ShowTableItem();

private:
    Ui::ShowTableItem *ui;
};

#endif // SHOWTABLEITEM_H
