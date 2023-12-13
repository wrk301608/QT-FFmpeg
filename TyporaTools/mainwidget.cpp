#include "mainwidget.h"
#include "ui_mainwidget.h"
#include"trayicon.h"
#include<QMessageBox>
#include<QCloseEvent>
#include"gifwidget.h"
#include"codecount.h"
#include"screenwidget.h"
#include<QFileDialog>
#include<QStandardPaths>
#include"qsettings.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    connect(ReadThread::getInstance(), &ReadThread::playState, this, &MainWidget::on_playState);
    connect(&m_timer, &QTimer::timeout, this, &MainWidget::on_timeout);

}

MainWidget::~MainWidget()
{
    delete ui;

}

//开始截屏的槽函数
void MainWidget::on_startCap_btn_clicked()
{
    //设置
//    ScreenWidget::Instance()->setParent(this);
    ScreenWidget::Instance()->setMainWidget(this);
    this->hide();
    //ScreenWidget全屏显示
    QTimer::singleShot(300, []() {
        ScreenWidget::Instance()->showFullScreen();
    });


}

void MainWidget::on_init_btn_clicked()
{
    //设置默认位置
    QString defaultPath = QApplication::applicationDirPath();
    QSettings settings("MyApp","MySettings");
    settings.setValue("DefaultPath",defaultPath);
    QString FileName = QFileDialog::getExistingDirectory(this,
                                                         "选择文件夹",
                                                         settings.value("DefaultPath").toString());

    ScreenWidget::setInitFileName(FileName);
}

void MainWidget::on_startRecord_btn_clicked()
{
    if(ui->startRecord_btn->text() == "开始录制")
    {
        if(!setSavePath())
        {
            return;
        }
        ReadThread::getInstance()->open("desktop");
    }

    else
    {
        ReadThread::getInstance()->close();
    }
}

void MainWidget::on_playState(ReadThread::PlayState state)
{
    if(state == ReadThread::play)
    {

        ui->startRecord_btn->setText("停止录制");
        m_timer.start(1000);
        ui->timeEdit->setTime(QTime(0, 0, 0, 0));     // 这里重置不要使用clear()
    }
    else
    {
        ui->startRecord_btn->setText("开始录制");

        m_timer.stop();
    }
}

bool MainWidget::setSavePath()
{
    QString strDefault = QString("%1/%2.mp4").arg(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation))
                                                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss"));
    QString strPath = QFileDialog::getSaveFileName(this, "视频保存到~",  strDefault,
                                                   "常用视频文件 (*.mp4 *.avi *.mov *.wmv *.flv *.h264 *.h265);;"
                                                   "其它文件格式 (*)");

    if(strPath.isEmpty()) return false;

    ui->line_path->setText(strPath);
    ReadThread::getInstance()->setPath(strPath);
    return true;
}


void MainWidget::on_timeout()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(1));
}



//处理代码统计的槽函数
void MainWidget::on_codeCount_btn_clicked()
{
    CodeCount::getInstance()->show();
}

//处理GIF制作的槽函数
void MainWidget::on_GIFMake_btn_clicked()
{
    this->hide();
    GIFWidget::getInstance()->setMainWidget(this);
    GIFWidget::getInstance()->setWindowFlags(GIFWidget::getInstance()->windowFlags() | Qt::WindowStaysOnTopHint);
    GIFWidget::getInstance()->show();
}


//重写关闭事件
void MainWidget::closeEvent(QCloseEvent *event)
{
    //弹出一个选择按钮，是否最小化程序到托盘运行
    QMessageBox::StandardButton button = QMessageBox::question(this,
                                                               "提示",
                                                               "是否最小化到系统托盘",
                                                               QMessageBox::Yes|QMessageBox::No,
                                                              QMessageBox::Yes);
    //不关闭程序，最小化程序到托盘运行
    if(button == QMessageBox::Yes)
    {
        //设置托盘所属的主窗体
        TrayIcon::getInstance()->setMainWidget(this);
        //忽略关闭事件
        event->ignore();
        //隐藏主窗口
        this->hide();
        //设置托盘可见
        TrayIcon::getInstance()->setVisible(true);
        TrayIcon::getInstance()->setToolTip("TyproaTools");
        TrayIcon::getInstance()->showMessage("TyproTools托盘运行",
                                             "已最小化");

    }
    //关闭程序
    else
    {
        event->accept();
    }
}
