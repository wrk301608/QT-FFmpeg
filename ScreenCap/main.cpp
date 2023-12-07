
#include "screencapwidget.h"
#include <QApplication>
#include<QTextCodec>
int main(int argc, char *argv[])
{
    //设置高DPI缩放因子的取整策略：向下取整
    //在高分辨屏幕上，Qt程序会对页面进行缩放，DPI是浮点数，转整数设置向下取整
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    QApplication a(argc, argv);
    //设置utf-8文本编解码器
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
    ScreenCapWidget w;
    //设置title
    w.setWindowTitle("屏幕截图Tools");
    w.show();
    return a.exec();
}
