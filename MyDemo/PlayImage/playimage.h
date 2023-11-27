#ifndef PLAYIMAGE_H
#define PLAYIMAGE_H

#include <QWidget>
#include<qmutex.h>
class PlayImage : public QWidget
{
    Q_OBJECT
public:
    explicit PlayImage(QWidget *parent = nullptr);

    //声明更新QImage格式的函数
    void updateImage(const QImage& image);
    //声明更新QPixmap格式的函数
    void updatePixmap(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
private:
    QPixmap m_pixmap;
    //维持一个互斥锁变量
    QMutex m_mutex;
};

#endif // PLAYIMAGE_H
