#include "screencapwidget.h"
#include "ui_screencapwidget.h"
#include"screenwidget.h"
#include"qdatetime.h"
#include"qfiledialog.h"
#include"qdebug.h"
#include"qsettings.h"
#include"trayicon.h"
#include"qmessagebox.h"

#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
ScreenCapWidget::ScreenCapWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScreenCapWidget)
{
    ui->setupUi(this);

}

ScreenCapWidget::~ScreenCapWidget()
{

    delete ui;
    exit(0);
}


//定义开始截图的槽函数
void ScreenCapWidget::on_start_btn_clicked()
{
    qDebug() <<"Q+E";
    //ScreenWidget全屏显示
    ScreenWidget::Instance()->showFullScreen();
}

void ScreenCapWidget::on_open_btn_clicked()
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

void ScreenCapWidget::closeEvent(QCloseEvent *event)
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
        TrayIcon::getInstance()->setToolTip("ScreenCap");
        TrayIcon::getInstance()->showMessage("ScrennCap托盘运行",
                                             "已最小化");

    }
    //关闭程序
    else
    {
        event->accept();
    }
}

