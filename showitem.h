#ifndef SHOWITEM_H
#define SHOWITEM_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui
{
    class ShowItem;
}

class ShowItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShowItem(QWidget* parent = nullptr);
    ~ShowItem();
    void setMusicName(QString musicName);
    void setMusicAuthor(QString musicAuthor);
    void setMusicPic(QString musicPicUrl);
    void setImageFromUrl(const QString& url, QLabel* label);

private:
    Ui::ShowItem* ui;
};

#endif // SHOWITEM_H
