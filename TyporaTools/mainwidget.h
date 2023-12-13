#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include<QTimer>
#include"readthread.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_startCap_btn_clicked();

    void on_init_btn_clicked();

    void on_startRecord_btn_clicked();

    void on_codeCount_btn_clicked();

    void on_GIFMake_btn_clicked();

    bool setSavePath();

    void on_timeout();

    void on_playState(ReadThread::PlayState state);

private:
    Ui::MainWidget *ui;
    QTimer m_timer;
private:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWIDGET_H
