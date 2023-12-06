#include "tray.h"
#include "ui_tray.h"
#include"trayicon.h"
#include"qmessagebox.h"
Tray::Tray(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Tray)
{
    ui->setupUi(this);
}

Tray::~Tray()
{
    delete ui;
}

void Tray::closeEvent(QCloseEvent *event)
{
    //弹出一个选择按钮，是否最小化程序到托盘运行
    QMessageBox::StandardButton button = QMessageBox::question(this,
                                                               "提示",
                                                               "是否最小化到系统托盘",
                                                               QMessageBox::Yes|QMessageBox::No,
                                                              QMessageBox::Yes);
    //不关闭程序，最小化程序到托盘运行
    if(button == QMessageBox::Yes)
    {
        //设置托盘所属的主窗体
        TrayIcon::Instance()->setMainWidget(this);
        //忽略关闭事件
        event->ignore();
        //隐藏主窗口
        this->hide();
        //设置托盘可见
        TrayIcon::Instance()->setVisible(true);
        //设置提示消息，看不见消息的是电脑开启了勿扰模式
        TrayIcon::Instance()->showMessage("自定义最小化托盘",
                                          "已最小化");
    }
    //关闭程序
    else
    {
        event->accept();
    }
}


