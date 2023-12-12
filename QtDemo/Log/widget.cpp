#include "widget.h"
#include "ui_widget.h"
#include<QApplication>
#include"mylog.h"
#include"QDebug"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //程序运行就安装上日志系统
    MyLog::getInstance()->installMessageHandler();
}

Widget::~Widget()
{
    //析构就卸载
    MyLog::getInstance()->uninstallMessageHandler();
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    //测试按钮
    qDebug() << "hello";
    qDebug() << "当前时间:" <<QTime::currentTime().toString("hh:mm:ss");
    qInfo() << QString("God bless you");

}
