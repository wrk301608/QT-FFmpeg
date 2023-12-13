#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include"myscreen.h"
#include <QWidget>
#include<QMenu>

class ScreenWidget : public QWidget
{
    Q_OBJECT
public:
    //声明一个单例模式
    static ScreenWidget* Instance();
    explicit ScreenWidget(QWidget *parent = nullptr);
    //声明对InitFileName值的设置
    static void setInitFileName(QString filename);
    //设置父类窗口
    void setMainWidget(QWidget* mainwidget);
    ~ScreenWidget();


private:
    //维护智能指针
    static QScopedPointer<ScreenWidget> self;
    QMenu* menu;//右键菜单对象
    myScreen* myscreen;
    QPixmap* fullScreen;//保存全屏对象
    QPixmap* bgScreen;//模糊背景图
    QPoint movPos;//拖拽开始的位置
    static QString initFileName;//默认的保存地址
    //维持父类的窗口
    QWidget* mainWidget;


private slots:
    //保存截图槽函数
    void saveScreen();
    //保存全屏截图槽函数
    void saveFullScreen();
    //另存为的槽函数
    void saveScreenOther();
    //另存全屏截图的槽函数
    void saveFullOther();
    //退出槽函数
    void closeScreen();

protected:
    //窗口被显示的时候的重写事件
    void showEvent(QShowEvent *) override;
    //重写绘图事件
    void paintEvent(QPaintEvent*) override;
    //重写鼠标事件
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void contextMenuEvent(QContextMenuEvent *) override;


signals:
};

#endif // SCREENWIDGET_H
