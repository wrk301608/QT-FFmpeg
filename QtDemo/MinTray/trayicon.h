#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include<QSystemTrayIcon>

//前向声明出来QMenu类
class QMenu;

class TrayIcon : public QObject
{
    Q_OBJECT
public:
    static TrayIcon* Instance();//声明单例
    explicit TrayIcon(QObject *parent = nullptr);
private:
    static QScopedPointer<TrayIcon> self;
    //维持所属的主窗体
    QWidget* mainWidget;
    //维持右键托盘的菜单功能
    QMenu* menu;
    //维持托盘对象
    QSystemTrayIcon* trayIcon;
    //是否直接退出的标志位
    bool exitDirect;
public:
    //设置是否直接退出，不是发生信号给主界面
    void setExitDirect(bool exitDirect);
    //设置提示信息
    void setToolTip(const QString& tip);
    //声明所属的主窗体
    void setMainWidget(QWidget* mainWidget);
    //声明显示消息
    void showMessage(const QString& title,
                     const QString& msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                     int msec = 5000
            );
    //对外获取和设置是否可见
    bool getVisible() const;
    void setVisible(bool visible);
public Q_SLOTS:
    //退出菜单
    void closeAll();
    //显示主窗体的菜单
    void showMainWidget();
private slots:
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
signals:
    void trayIconExit();
};

#endif // TRAYICON_H
