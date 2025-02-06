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
