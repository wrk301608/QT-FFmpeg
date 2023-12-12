#ifndef MYLOG_H
#define MYLOG_H

#include <iostream>
#include <QDateTime>
#include <QMutexLocker>
#include <QDir>
#include <QTimer>
#include <QTextStream>


//最大保存文件大小
const int g_logLimitSize = 5;

class MyLog
{
public:
    MyLog();
    ~MyLog();

    static MyLog* getInstance();

    //消息处理函数
    static void messageHandler(QtMsgType type,
                                   const QMessageLogContext& context,
                                   const QString& msg);
public:
    //打开并备份之前的日志文件
    void openAndBackupLogFile();
    void checkLogFiles();
    void autoDeleteLog();
    //安装消息处理函数
    void installMessageHandler();

    //卸载消息处理函数，并释放资源
    void uninstallMessageHandler();
private:
    //日志文件夹目录
    QDir logDir;
    //重命名日志文件使用的定时器
    QTimer renameLogFileTimer;
    //刷新输出到日志文件的定时器
    QTimer flushLogFileTimer;
    //日志文件的创建时间
    QDate logFileCreateDate;

    //日志文件
    static QFile* logFile;
    //输出日志
    static QTextStream* logOut;
    //日志锁
    static QMutex logMutex;
    static QScopedPointer<MyLog> self;
};

#endif // MYLOG_H
