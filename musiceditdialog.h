#ifndef MUSICEDITDIALOG_H
#define MUSICEDITDIALOG_H

#include <QDialog>
#include <QFileDialog>

#include "music.h"
#include "usemysql.h"

namespace Ui {
class MusicEditDialog;
}

class MusicEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MusicEditDialog(QWidget *parent = nullptr);
    void initMusicEditDialog(Music* music);
    ~MusicEditDialog();

signals:
    void updateMusicList();

private slots:

    void on_pushButton_cancel_clicked();

    void on_pushButton_url_clicked();

    void on_pushButton_picurl_clicked();

    void on_pushButton_ok_clicked();

private:
    Music* m_music;
    Ui::MusicEditDialog *ui;
};

#endif // MUSICEDITDIALOG_H
