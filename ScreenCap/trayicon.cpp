#include "trayicon.h"
#include"qmutex.h"
#include"qapplication.h"
#include"qmenu.h"
#include"screenwidget.h"
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
}



//定义设置所属的主窗体
void TrayIcon::setMainWidget(QWidget *mainWidget)
{
    this->mainWidget = mainWidget;


    menu->addAction("主界面",this,SLOT(showMainWidget()));
    menu->addAction("开始截屏",this,SLOT(StartCap()));
    menu->addAction("退出",this,SLOT(closeAll()));

    trayIcon->setContextMenu(menu);

}
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
bool TrayIcon::getVisible() const
{
    return trayIcon->isVisible();
}

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
}

//托盘开始截屏
void TrayIcon::StartCap()
{
    ScreenWidget::Instance()->showFullScreen();
}


