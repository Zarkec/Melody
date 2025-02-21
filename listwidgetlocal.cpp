#include "listwidgetlocal.h"
#include "ui_widget.h"


void ListWidgetLocal::contextMenuEvent(QContextMenuEvent *event)
{
    // 创建右键菜单对象
    QMenu menu;
	menu.setStyleSheet("QMenu{background-color: rgb(33, 33, 33);color: rgb(219, 219, 219);}"
		"QMenu::item:selected{background-color: rgb(50, 50, 50);}");
    // 添加菜单项
    QAction *action1 = menu.addAction("删除");
    QAction *action2 = menu.addAction("编辑");

    // 连接菜单项的触发信号到相应槽函数（这里简单打印，实际可替换为具体功能）
    connect(action1, &QAction::triggered, [=]() {
        //qDebug() << "删除被点击";
		//获取当前点击的item
		QListWidgetItem* item = currentItem();
		ShowItem* showItem = qobject_cast<ShowItem*>(itemWidget(item));
		Music music = showItem->getMusic();
        qDebug()<<"==========================="<<music.id();
		//qDebug() << "当前点击的列表项文本：" << itemText;
		UseMySQL* useMySQL = UseMySQL::instance();
        useMySQL->deleteMusicFromMysql(music);
        emit updateListWidgetLocal();
    });
    connect(action2, &QAction::triggered, [=]() {
        //qDebug() << "菜单项2被点击";
        MusicEditDialog musicEditDialog;
        musicEditDialog.setWindowTitle("编辑歌曲信息");
        musicEditDialog.setFixedSize(346, 220);
        QListWidgetItem* item = currentItem();
        musicEditDialog.setWindowIcon(QIcon(":/res/img/icon.png"));
        ShowItem* showItem = qobject_cast<ShowItem*>(itemWidget(item));
        Music music = showItem->getMusic();
        musicEditDialog.initMusicEditDialog(&music);
        connect(&musicEditDialog, &MusicEditDialog::updateMusicList, this, [=]() 
        {
            emit updateListWidgetLocal();
        });
        musicEditDialog.exec();
        
    });

    // 获取当前鼠标点击位置对应的列表项索引
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        // 如果点击位置有有效列表项，可根据需要在此添加针对该项的操作
        // 例如获取该项的文本：QString itemText = item(index.row())->text();
        //QString itemText = item(index.row())->text();
        //ShowItem* showItem = qobject_cast<ShowItem*>(ui->listWidgetLocal->itemWidget(item));
        //Music music = showItem->getMusic();
		//qDebug() << "当前点击的列表项文本：" << itemText;
    }

    // 在鼠标右键点击位置弹出菜单
    menu.exec(event->globalPos());

}
