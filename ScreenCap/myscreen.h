#ifndef SCREEN_H
#define SCREEN_H
#include<QSize>
#include<QPoint>
class myScreen
{
public:
    //截图区域状态
    enum STATUS{SELECT,MOV,SET_W_H};
    myScreen(){};//默认构造，不用
    myScreen(QSize size);//带QSize的构造函数
    //声明set
    void setStart(QPoint pos);
    void setEnd(QPoint pos);
    void setStatus(STATUS status);

    //声明get
    int getWidth();//get截屏的宽
    int getHeight();//get截屏的高
    STATUS getStatus();
    QPoint getStart();
    QPoint getEnd();
    QPoint getLeftUp();//get截屏区域左上角point
    QPoint getRightDown();//get截屏区域右下角point

    //声明检查坐标点是否在截图区域内
    bool isInArea(QPoint pos);
    //声明按照坐标移动截图区域
    void move(QPoint p);
private:
    //维持屏幕大小
    int maxWidth,maxHeight;
    //维持截屏区域位置
    QPoint leftUpPos,rightDownPos;
    //维持鼠标位置
    QPoint startPos,endPos;
    //维持截图区域状态
    STATUS status;
private:
    //声明比较左上角，右下角的位置函数，使lu一直在rd的左下
    void cmpPoint(QPoint& lu,QPoint& rd);
};

#endif // SCREEN_H
