#include "playimage.h"
#include<QPainter>
PlayImage::PlayImage(QWidget *parent) : QWidget(parent)
{
    //构造函数里设置背景为黑色
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background,Qt::black);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
}

//定义Image的更新函数
void PlayImage::updateImage(const QImage& image)
{
    //因为QPixmap更适合绘图事件，而且更稳定快速，这里转为QPixmap格式
    updatePixmap(QPixmap::fromImage(image));
}

//定义Pixmap的更新函数
void PlayImage::updatePixmap(const QPixmap &pixmap)
{
    //更新m_pixmap的值前先加锁
    m_mutex.lock();
    m_pixmap = pixmap.scaled(this->size(),Qt::KeepAspectRatio);
    m_mutex.unlock();
    //更新完m_pixmap的值后发送重绘信号
    update();
}

//重写绘图事件
void PlayImage::paintEvent(QPaintEvent *event)
{
    //非空就重绘
    if(!m_pixmap.isNull())
    {
        QPainter painter(this);
        //居中绘制
        int x = (this->width()-m_pixmap.width())/2;
        int y = (this->height()-m_pixmap.height())/2;
        painter.drawPixmap(x,y,m_pixmap);
    }
    //确保绘图事件成功运行
    QWidget::paintEvent(event);
}
