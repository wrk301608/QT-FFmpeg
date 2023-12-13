#ifndef GIFWIDGET_H
#define GIFWIDGET_H

#include <QDialog>
#include"gif.h"
#include<QCloseEvent>
class QLineEdit;
class QLabel;


class GIFWidget : public QDialog
{
    Q_OBJECT

    Q_PROPERTY(int borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)
public:
    static GIFWidget* getInstance();

    explicit GIFWidget(QWidget* parent = 0);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent*);
private:
    static QScopedPointer<GIFWidget> self;
    QWidget *widgetTop;         //标题栏
    QWidget *widgetMain;        //中间部分
    QWidget *widgetBottom;      //底部栏
    QLineEdit *txtFps;          //帧率输入框
    QLineEdit *txtWidth;        //宽度输入框
    QLineEdit *txtHeight;       //高度输入框
    QPushButton *btnStart;      //开始按钮
    QLabel *labStatus;          //显示状态信息

    int fps;//帧数
    int borderWidth;//边框宽度
    QColor bgColor;//背景颜色

    int fpsCount;//帧数计数
    QString fileName;//保存的文件名称
    QRect rectGif;//制作区域
    QTimer* timer;//截屏定时器

    Gif gif;//gif类对象
    Gif::GifWriter* gifWriter;//gif写入对象

    //维持所属的主窗体
    QWidget* mainWidget;

public:
    int getBorderWidth() const;
    QColor getBgColor() const;

    //对外暴露的接口
    //所属的主窗体
    void setMainWidget(QWidget* mainWidget);

private slots:
    void initControl();
    void initForm();
    void saveImage();
    void record();
    void closeAll();
    void resizeForm();
public Q_SLOTS:
    void setBorderWidth(int borderWidth);
    void setBgColor(const QColor& bgColor);
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // GIFWIDGET_H
