#ifndef LISTWIDGETLOCAL_H
#define LISTWIDGETLOCAL_H

#include <QListWidget>
#include <QObject>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QContextMenuEvent>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>


#include "usemysql.h"
#include "showitem.h"
#include "widget.h"


class ListWidgetLocal : public QListWidget
{
    Q_OBJECT
public:
    ListWidgetLocal(QWidget *parent = nullptr) : QListWidget(parent) {};
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // LISTWIDGETLOCAL_H
