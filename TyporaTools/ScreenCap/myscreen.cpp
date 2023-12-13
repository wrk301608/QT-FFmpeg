#include "myscreen.h"

//定义位置比较函数，使lu永远在rd的左下
void myScreen::cmpPoint(QPoint &lu, QPoint &rd)
{
    QPoint l = lu;
    QPoint r = rd;
    //lu在rd的左边
    if(l.x() <= r.x())
    {
        if(l.y() <= r.y())
        {
            //lu在rd的左下
        }
        else
        {
            //l在r的左上
            lu.setY(r.y());
            rd.setY(l.y());
            //交换lu和rd的y坐标，仍保持lu在rd的左下
        }
    }
    //lu在rd的右边
    else
    {
        //lu在rd的右下
        if(l.y() < r.y())
        {
            lu.setX(r.x());
            rd.setX(l.x());
            //交换lu和rd的x坐标，使lu在rd的左下
        }
        //lu在rd的右上
        else
        {
            //完全相反，交换，使lu在rd的左下
            QPoint tmp;
            tmp = lu;
            lu = rd;
            rd = tmp;
        }
    }
}

//定义带QSize的构造函数
myScreen::myScreen(QSize size)
{

    //初始化屏幕size为主屏幕的size
    maxWidth = size.width();
    maxHeight = size.height();
    //初始化
    startPos = QPoint(-1,-1);
    endPos = startPos;
    leftUpPos = startPos;
    rightDownPos = startPos;
    status = SELECT;//选择状态
}

//定义get
int myScreen::getWidth()
{
    return maxWidth;
}

int myScreen::getHeight()
{
    return maxHeight;
}

myScreen::STATUS myScreen::getStatus()
{
    return status;
}

QPoint myScreen::getEnd()
{
    return endPos;
}

QPoint myScreen::getStart()
{
    return startPos;
}

QPoint myScreen::getLeftUp()
{
    return leftUpPos;
}

QPoint myScreen::getRightDown()
{
    return rightDownPos;
}
//定义set
void myScreen::setStatus(STATUS status)
{
    this->status = status;
}

void myScreen::setEnd(QPoint pos)
{
    endPos = pos;
    leftUpPos = startPos;
    rightDownPos = endPos;
    cmpPoint(leftUpPos,rightDownPos);

}
void myScreen::setStart(QPoint pos)
{
    startPos = pos;
}

//定义isInArea函数
bool myScreen::isInArea(QPoint pos)
{
    //在区域内
    if(pos.x() > leftUpPos.x()
            && pos.x() < rightDownPos.x()
            &&pos.y() > leftUpPos.y()
            &&pos.y() < rightDownPos.y()
            )
    {
        return true;
    }
    return false;

}

//定义move函数
void myScreen::move(QPoint p)
{
    //计算move后的四个点坐标
    int lx = leftUpPos.x() + p.x();
    int ly = leftUpPos.y() + p.y();
    int rx = rightDownPos.x() + p.x();
    int ry = rightDownPos.y() + p.y();
    //确保移动后的截屏不会超出屏幕范围
    if(lx < 0)
    {
        lx = 0;
        rx -= p.x();
    }
    if(ly < 0)
    {
        ly = 0;
        ry -= p.y();
    }
    if(rx > maxWidth)
    {
        rx = maxWidth;
        lx -= p.x();
    }
    if(ry > maxHeight)
    {
        ry = maxHeight;
        ly -= p.y();
    }

    //更新移动后的值
    leftUpPos = QPoint(lx,ly);
    rightDownPos = QPoint(rx,ry);
    startPos = leftUpPos;
    endPos = rightDownPos;
}
