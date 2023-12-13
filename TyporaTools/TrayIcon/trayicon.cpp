#include "trayicon.h"
#include"qmutex.h"
#include"qapplication.h"
#include"qmenu.h"
#include<QDebug>
#include"gifwidget.h"
#include"codecount.h"
#include"screenwidget.h"
#include<QStandardPaths>
#include<QFileDialog>
#include<QWidget>
#include<windows.h>
QScopedPointer<TrayIcon> TrayIcon::self;

//定义单例
TrayIcon* TrayIcon::getInstance()
{
    if(self.isNull())
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if(self.isNull())
        {
            self.reset(new TrayIcon);
        }
    }

    return self.data();
}

TrayIcon::TrayIcon(QObject *parent) : QObject(parent)
{
    mainWidget = 0;
    //实例化托盘对象
    trayIcon = new QSystemTrayIcon(this);
    //设置托盘图标
    trayIcon->setIcon(QIcon(":/mintray.ico"));

    //实例化托盘菜单
    menu = new QMenu;

//    connect(ReadThread::getInstance(), &ReadThread::playState, this, &TrayIcon::on_playState);

}

//定义设置所属的主窗体
void TrayIcon::setMainWidget(QWidget *mainWidget)
{
    this->mainWidget = mainWidget;


    actionScreenCap = new QAction("开始截屏", this);
    actionStartRecord = new QAction("开始录制", qApp);
    actionStopRecord = new QAction("停止录制", this);
    actionGIFMake = new QAction("GIF制作", this);
    actionCodeCount = new QAction("代码统计", this);
    actionDefaultPosition = new QAction("默认位置", this);
    actionShowMain = new QAction("主界面", this);
    actionExit = new QAction("退出", this);

    menu->addAction(actionScreenCap);
    menu->addAction(actionStartRecord);
    menu->addAction(actionStopRecord);
    menu->addAction(actionGIFMake);
    menu->addAction(actionCodeCount);
    menu->addAction(actionDefaultPosition);
    menu->addAction(actionShowMain);
    menu->addAction(actionExit);


//    connect(actionScreenCap, &QAction::triggered, this, &TrayIcon::TrayScreenCap);
//    connect(actionStartRecord, &QAction::triggered, this,&TrayIcon::TrayVideoCap);
    connect(actionGIFMake, &QAction::triggered, this, &TrayIcon::TrayGIFMake);
    connect(actionCodeCount, &QAction::triggered, this, &TrayIcon::TrayCodeCount);
    connect(actionShowMain, &QAction::triggered, this, &TrayIcon::showMainWidget);
    connect(actionExit, &QAction::triggered, this, &TrayIcon::closeAll);


    trayIcon->setContextMenu(menu);

}



//void TrayIcon::on_playState(ReadThread::PlayState state)
//{
//    if(state == ReadThread::play)
//    {

//        actionStartRecord->setText("停止录制");
//        m_timer.start(1000);

//    }
//    else
//    {
//        actionStartRecord->setText("开始录屏");
//        m_timer.stop();
//    }
//}





//设置提示信息
void TrayIcon::setToolTip(const QString &tip)
{
    trayIcon->setToolTip(tip);
}

//设置气泡信息
void TrayIcon::showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon, int msec)
{
    trayIcon->showMessage(title,msg,icon,msec);
}

//实现显示托盘
//获取显示状态
bool TrayIcon::getVisible() const
{
    return trayIcon->isVisible();
}


//设置显示状态
void TrayIcon::setVisible(bool visible)
{
    trayIcon->setVisible(visible);
}

//实现槽函数
//退出菜单功能
void TrayIcon::closeAll()
{
    //隐藏，删掉，退出
    trayIcon->hide();
    trayIcon->deleteLater();
    qApp->exit();
}
//回显主界面
void TrayIcon::showMainWidget()
{

    if(mainWidget)
    {
        mainWidget->showNormal();
        mainWidget->activateWindow();
    }

    //会到主页面后，清除旧的菜单项
    menu->clear();

//    menu->addAction(actionExit);
//    trayIcon->setContextMenu(menu);

}

//托盘GIF制作
void TrayIcon::TrayGIFMake()
{
    GIFWidget::getInstance()->setWindowFlags(GIFWidget::getInstance()->windowFlags() | Qt::WindowStaysOnTopHint);
    GIFWidget::getInstance()->show();
}

//托盘代码统计
void TrayIcon::TrayCodeCount()
{
    CodeCount::getInstance()->show();
}

//托盘截屏
void TrayIcon::TrayScreenCap()
{

    //ScreenWidget全屏显示
    ScreenWidget::Instance()->showFullScreen();
}



////托盘录制
//void TrayIcon::TrayVideoCap()
//{
////    if(actionStartRecord->text() == "开始录制")
////    {
//        if(!setSavePath())
//        {
//            return;
//        }
//        ReadThread::getInstance()->open("desktop");
////    }
////    else
////    {
////        ReadThread::getInstance()->close();
////        ReadThread::getInstance()->wait();
////    }
//}


//bool TrayIcon::setSavePath()
//{
//    QString strDefault = QString("%1/%2.mp4").arg(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation))
//                                                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss"));
//    QString strPath = QFileDialog::getSaveFileName(qApp->activeWindow(), "视频保存到~",  strDefault,
//                                                   "常用视频文件 (*.mp4 *.avi *.mov *.wmv *.flv *.h264 *.h265);;"
//                                                   "其它文件格式 (*)");

//    if(strPath.isEmpty()) return false;
//    ReadThread::getInstance()->setPath(strPath);
//    return true;
//}
