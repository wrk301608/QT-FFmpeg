#ifndef SCREENCAPWIDGET_H
#define SCREENCAPWIDGET_H

#include <QWidget>
#include<QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class ScreenCapWidget; }
QT_END_NAMESPACE

class ScreenCapWidget : public QWidget
{
    Q_OBJECT

public:
    ScreenCapWidget(QWidget *parent = nullptr);
    ~ScreenCapWidget();

private slots:
    void on_start_btn_clicked();

    void on_open_btn_clicked();



private:
    Ui::ScreenCapWidget *ui;

private:
    void closeEvent(QCloseEvent *event) override;



};
#endif // SCREENCAPWIDGET_H
