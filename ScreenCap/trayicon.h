#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include<QSystemTrayIcon>
class QMenu;

class TrayIcon : public QObject
{
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = nullptr);
    static TrayIcon* getInstance();
private:
    static QScopedPointer<TrayIcon> self;
    //维持所属的主窗体
    QWidget* mainWidget;
    //维持托盘对象
    QSystemTrayIcon* trayIcon;
    //维持托盘菜单
    QMenu* menu;
public:
    //所属的主窗体
    void setMainWidget(QWidget* mainWidget);
    //获取可见和不可见
    bool getVisible() const;
    void setVisible(bool visible);
    //设置鼠标放上去的提示信息
    void setToolTip(const QString& tip);
    //设置最小化托盘后的系统提示信息
    void showMessage(const QString& title,
                     const QString& msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                     int msec = 5000);
public Q_SLOTS:
    //退出
    void closeAll();
    //回显主窗体
    void showMainWidget();
    //开始截屏
    void StartCap();
private slots:

signals:

};

#endif // TRAYICON_H
