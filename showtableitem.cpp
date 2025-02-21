#include "showtableitem.h"
#include "ui_showtableitem.h"

ShowTableItem::ShowTableItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShowTableItem)
{
    ui->setupUi(this);
}

ShowTableItem::~ShowTableItem()
{
    delete ui;
}

void ShowTableItem::initShowTableItem(Music* music)
{
    qDebug() << "ShowTableItem::initShowTableItem";
    ui->label_author->setText(music->author());
    ui->label_musicname->setText(music->name());
    ui->label_pic->setPixmap(QPixmap(music->picurl()));
}
