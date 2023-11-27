#include "widget.h"
#include "ui_widget.h"
#include<QFileDialog>
#include<QDebug>
#include<QMessageBox>
#include"playimage.h"
#include<QTimer>
#include<QEventLoop>
#define LOG 1
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("图片播放器"));
    //设置窗口固定
    int width = 800;
    int height = 500;
    this->setFixedSize(width,height);


}

Widget::~Widget()
{
    delete ui;
}

//定义QString转QPixmap的函数
QPixmap Widget::stringToPixmap(QString fileName)
{
    QPixmap pixmap(fileName);
    return pixmap;
}

//定义阻塞函数
void Widget::sleepMsec(int msec)
{
    //创建事件循环对象
    QEventLoop loop;
    //创建计时器对象
    QTimer timer;
    timer.setSingleShot(true);

    //连接计时器的超时信号到事件循环的退出槽函数
    connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);

    //启动计时器
    timer.start(msec);
    //阻塞
    loop.exec();

}

//定义图片打开的槽函数
void Widget::on_open_btn_clicked()
{
    fileNames = QFileDialog::getOpenFileNames(this,//打开的父窗口
                                              "选择图片",//文件对话框标题
                                              "/",//从根目录开始
                                              "图像文件(*.jpg *.png *.bmp)");
    //图片选择失败
    if(fileNames.isEmpty())
    {
#if LOG
        qDebug() <<"图片路径为空";
#endif
       QMessageBox::warning(this,"警告","请选择图片");
    }

    //选择成功后，调式输出图片路径的值,并默认显示第一张图片
#if LOG
    foreach(const QString& fileName,fileNames)
    {
        qDebug() << "选择的图片路径：" << fileName;
    }
#endif
    //默认显示第一张图片
    picTotal = fileNames.count();

    QPixmap initPixmap = stringToPixmap(fileNames.first());
    if(!initPixmap.isNull())
    {
        //转换成功
        ui->playImage->updatePixmap(initPixmap);
    }
    else
    {
        //转换失败
#if LOG
        qDebug() << "转为QPixmap失败";
#endif
    }

}

void Widget::on_start_btn_clicked()
{
    play = true;
    //没有图片可以播放
    if(picTotal == 0)
    {
#if LOG
        qDebug() << "开始播放前没有图片";
#endif
        QMessageBox::warning(this,"警告","没有图片播放");
    }
    //有图片播放
    //先打开暂停播放的功能
    ui->pause_btn->setEnabled(true);
    while(play&&picTotal!=0)
    {
        picIndex = (picIndex + 1)%picTotal;
        //隔一会再更新图片
        sleepMsec(1000);
        ui->playImage->updatePixmap(stringToPixmap(fileNames.at(picIndex)));
    }

}

void Widget::on_pause_btn_clicked()
{
    play = false;
    ui->pause_btn->setEnabled(false);

}

void Widget::on_previous_clicked()
{
    picIndex = (picIndex - 1 + picTotal)%picTotal;
    //更新图片
    ui->playImage->updatePixmap(stringToPixmap(fileNames.at(picIndex)));
}


void Widget::on_next_btn_clicked()
{
    picIndex = (picIndex + 1)%picTotal;
    //更新图片
    ui->playImage->updatePixmap(stringToPixmap(fileNames.at(picIndex)));
}
