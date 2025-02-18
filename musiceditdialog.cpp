#include "musiceditdialog.h"
#include "ui_musiceditdialog.h"

MusicEditDialog::MusicEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MusicEditDialog)
{
    ui->setupUi(this);
}

MusicEditDialog::~MusicEditDialog()
{
    delete ui;
}

void MusicEditDialog::initMusicEditDialog(Music *music)
{
    m_music = music;
    ui->lineEdit_musicname->setText(music->name());
    ui->lineEdit_author->setText(music->author());
    ui->lineEdit_album->setText(music->album());
    ui->lineEdit_url->setText(music->url());
    ui->lineEdit_picurl->setText(music->picurl());
}


void MusicEditDialog::on_pushButton_cancel_clicked()
{
    close();
}


void MusicEditDialog::on_pushButton_url_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("编辑歌曲地址"), " ", tr("音乐文件(*.mp3)"));
    if (!filePath.isEmpty()) {
        ui->lineEdit_url->setText(filePath);
    }

}


void MusicEditDialog::on_pushButton_picurl_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("编辑封面地址"), " ", tr("图片文件(*.jpg *.png)"));
    if (!filePath.isEmpty()) {
        ui->lineEdit_picurl->setText(filePath);
    }

}


void MusicEditDialog::on_pushButton_ok_clicked()
{
    UseMySQL* useMySQL = new UseMySQL();
    Music music;
    music.setId(m_music->id());
    music.setName(ui->lineEdit_musicname->text());
    music.setAuthor(ui->lineEdit_author->text());
    music.setAlbum(ui->lineEdit_album->text());
    music.setUrl(ui->lineEdit_url->text());
    music.setPicurl(ui->lineEdit_picurl->text());
    useMySQL->updateMusicToMysql(music);
    emit updateMusicList();
    close();
}

