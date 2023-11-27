#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    QPixmap stringToPixmap(QString fileName);
    //声明阻塞函数
    void sleepMsec(int msec);
private slots:
    void on_open_btn_clicked();

    void on_start_btn_clicked();

    void on_pause_btn_clicked();

    void on_previous_clicked();



    void on_next_btn_clicked();

private:
    Ui::Widget *ui;
    //维持一个图片路径的列表
    QStringList fileNames;
    //维持一个图片索引值
    int picIndex = 0;
    //维持一个图片总数值
    int picTotal = 0;
    //控制播放标志位
    bool play = true;
};
#endif // WIDGET_H
