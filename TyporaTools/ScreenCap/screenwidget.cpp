#include "screenwidget.h"
#include"qscreen.h"
#include"qapplication.h"
#include"qmutex.h"
#include"qdatetime.h"
#include"qfiledialog.h"
#include"qpainter.h"
#include"qevent.h"
#include"qdebug.h"

#define LOG 0
#define deskGeometry qApp->primaryScreen()->geometry()
#define deskGeometry2 qApp->primaryScreen()->availableGeometry()
#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

QString ScreenWidget::initFileName = NULL;
QScopedPointer<ScreenWidget> ScreenWidget::self;

//定义单例模式，确保截屏的时候只能有一个
ScreenWidget* ScreenWidget::Instance()
{
    //还没有创建实例
    if(self.isNull())
    {
        //加把锁,只能有一个线程访问
        static QMutex mutex;
        //自动加解锁
        QMutexLocker locker(&mutex);
        //再次判断有没有实例，防止等待的时间中有线程获取到实例了
        if(self.isNull())
        {
            self.reset(new ScreenWidget);
        }
    }
    return self.data();

}


//定义设置初始位置
void ScreenWidget::setInitFileName(QString filename)
{
    initFileName = filename;
#if LOG
    qDebug() << initFileName;
#endif
}

//定义默认构造函数
ScreenWidget::ScreenWidget(QWidget *parent) : QWidget(parent)
{
    //没有设置要给initFileName一个初始值
    if(initFileName.isNull())
    {
        initFileName = qApp->applicationDirPath();
    }
    //这里不用ui文件了，直接在代码生成
    //创建一个菜单文件
    menu = new QMenu(this);
    //添加菜单的功能
    menu->addAction("保存当前的截图",this,SLOT(saveScreen()));
    menu->addAction("保存全屏截图",this,SLOT(saveFullScreen()));
    menu->addAction("截图另存为",this,SLOT(saveScreenOther()));
    menu->addAction("全屏截图另存为",this,SLOT(saveFullOther()));
    menu->addAction("退出截图",this,SLOT(closeScreen()));
    //获取屏幕大小
    myscreen = new myScreen(deskGeometry.size());

    //保存全屏对象
    fullScreen = new QPixmap();


}

//设置父类窗口
void ScreenWidget::setMainWidget(QWidget* mainwidget)
{
    this->mainWidget = mainwidget;
}

ScreenWidget::~ScreenWidget()
{
    exit(0);
}




//重写退出截图槽函数
void ScreenWidget::closeScreen()
{
    menu->hide();

    if(mainWidget != NULL)
    {
        mainWidget->show();
        mainWidget->raise();  // 将主界面置于最前方
        mainWidget->activateWindow();  // 激活主界面窗口
    }
   close();
}

//重写鼠标事件
//重写鼠标移动事件
void ScreenWidget::mouseMoveEvent(QMouseEvent *e)
{
    //在选择状态
    if(myscreen->getStatus() == myScreen::SELECT)
    {
        myscreen->setEnd(e->pos());
    }
    //在移动状态
    else if(myscreen->getStatus() == myScreen::MOV)
    {
        //计算鼠标偏移量
        QPoint p(e->x() - movPos.x(),e->y() - movPos.y());
        myscreen->move(p);
        movPos = e->pos();//保存上一次鼠标的位置
    }
    //触发窗口的更新,重新绘制屏幕截图和矩形框
    this->update();
}
//重写鼠标按下事件
void ScreenWidget::mousePressEvent(QMouseEvent *e)
{
    int status = myscreen->getStatus();
    //选择区域的状态
    if(status == myScreen::SELECT)
    {
        //把鼠标按下的位置设置为开始位置
        myscreen->setStart(e->pos());
    }
    //拖拽截屏
    else if(status == myScreen::MOV)
    {
        //鼠标不在截屏的区域内，是要重新选择截屏区域
        if(myscreen->isInArea(e->pos()) == false)
        {
            //新按下的位置设置为开始位置，并重置状态为选择
            myscreen->setStart(e->pos());
            myscreen->setStatus(myScreen::SELECT);
        }
        //在截屏区域内，是要拖拽截屏
        else
        {
            //开始移动的起始位置就是现在鼠标按下的位置
            movPos = e->pos();
            this->setCursor(Qt::SizeAllCursor);
        }
    }
    this->update();
}
//重写鼠标松开事件
void ScreenWidget::mouseReleaseEvent(QMouseEvent *)
{
    if(myscreen->getStatus() == myScreen::SELECT)
    {
        myscreen->setStatus(myScreen::MOV);
    }
    else if(myscreen->getStatus() == myScreen::MOV)
    {
        this->setCursor(Qt::ArrowCursor);
    }
}
//重写鼠标右键事件
void ScreenWidget::contextMenuEvent(QContextMenuEvent *)
{
    //弹出菜单
    this->setCursor(Qt::ArrowCursor);
    menu->exec(cursor().pos());
}
//重写绘图事件
void ScreenWidget::paintEvent(QPaintEvent *)
{
    //获取起始位置
    int x = myscreen->getLeftUp().x();
    int y = myscreen->getLeftUp().y();
    //获取宽高
    int w = myscreen->getRightDown().x() - x;
    int h = myscreen->getRightDown().y() - y;

    QPainter painter(this);

    //设置笔的属性
    QPen pen;
    pen.setColor(Qt::green);
    pen.setWidth(2);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    //将用灰色填充后的背景图片绘制到当前屏幕上，全屏状态
    painter.drawPixmap(0, 0, *bgScreen);

    if(w!=0 && h!=0)
    {
        //从全屏截图从复制出来制定的截屏，绘制到屏幕上
        painter.drawPixmap(x,y,fullScreen->copy(x,y,w,h));
    }

    //绘制矩形框，并显示参数
    painter.drawRect(x,y,w,h);

    pen.setColor(Qt::yellow);
    painter.setPen(pen);
    painter.drawText(x + 2, y - 8, tr("截图范围：( %1 x %2 ) - ( %3 x %4 )  图片大小：( %5 x %6 )")
                     .arg(x).arg(y).arg(x + w).arg(y + h).arg(w).arg(h));
}

//重写窗口被显示的事件
void ScreenWidget::showEvent(QShowEvent *)
{
    //设置截屏区域初始位置
    QPoint point(-1,-1);
    myscreen->setStart(point);
    myscreen->setEnd(point);

    //获取当前主屏幕对象
    QScreen* pscreen = QApplication::primaryScreen();
    //调用QScreen的grabwindow进行全屏截图获得全屏图像
    *fullScreen = pscreen->grabWindow(0,//要捕获的窗口的标识符，0表示整个屏幕
                                      0,//捕获区域的起始横坐标
                                      0,//捕获区域的起始纵坐标
                                      myscreen->getWidth(),//捕获区域的宽度为屏幕的宽度
                                      myscreen->getHeight());//捕获区域的高度为屏幕的高度

    //设置透明度实现模糊背景
    //以屏幕的宽高创建pix
    QPixmap pix(myscreen->getWidth(),myscreen->getHeight());
    //用灰色填充pix
    pix.fill((QColor(160,160,160,//rgb值表示灰色
                     200//透明度为200
                     )));
    //用保存的全屏对象实例化背景
    bgScreen = new QPixmap(*fullScreen);
    QPainter p(bgScreen);//将bgScreen与绘图事件关联
    p.drawPixmap(0,0,pix);//把灰色模糊绘制到背景图片上
}


//定义保存当前截图的槽函数
void ScreenWidget::saveScreen()
{
#if LOG
    qDebug() <<"没有设置默认位置";
    qDebug() << initFileName;
#endif
    int x = myscreen->getLeftUp().x();
    int y = myscreen->getLeftUp().y();
    int w = myscreen->getRightDown().x() - x;
    int h = myscreen->getRightDown().y() - y;

    QString fileName = QString("%1/screen_%2.png").arg(initFileName).arg(STRDATETIME);
    fullScreen->copy(x, y, w, h).save(fileName, "png");
    close();
}
//定义保存全屏截图的槽函数
void ScreenWidget::saveFullScreen()
{
    QString fileName = QString("%1/full_%2.png").arg(initFileName).arg(STRDATETIME);
    fullScreen->save(fileName, "png");
    close();
}
//定义另存为的槽函数
void ScreenWidget::saveScreenOther()
{
    QString name = QString("%1.png").arg(STRDATETIME);
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片", name, "png Files (*.png)");
    if (!fileName.endsWith(".png")) {
        fileName += ".png";
    }

    if (fileName.length() > 0) {
        int x = myscreen->getLeftUp().x();
        int y = myscreen->getLeftUp().y();
        int w = myscreen->getRightDown().x() - x;
        int h = myscreen->getRightDown().y() - y;
        fullScreen->copy(x, y, w, h).save(fileName, "png");
        close();
    }
}
//定义全屏截屏另存为的槽函数
void ScreenWidget::saveFullOther()
{
    QString name = QString("%1.png").arg(STRDATETIME);
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片", name, "png Files (*.png)");
    if (!fileName.endsWith(".png")) {
        fileName += ".png";
    }

    if (fileName.length() > 0) {
        fullScreen->save(fileName, "png");
        close();
    }
}
