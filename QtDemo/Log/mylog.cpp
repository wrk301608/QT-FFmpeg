#include "mylog.h"
#include<QDebug>
#include<QTextCodec>

#define LOG 1
//初始化静态变量
QMutex MyLog::logMutex;
QFile* MyLog::logFile = NULL;
QTextStream* MyLog::logOut = NULL;
QScopedPointer<MyLog> MyLog::self;

//定义单例模式
MyLog* MyLog::getInstance()
{
    //还没有创建实例
    if(self.isNull())
    {
        //加把锁,只能有一个线程访问
        static QMutex mutex;
        //自动加解锁
        QMutexLocker locker(&mutex);
        //再次判断有没有实例，防止等待的时间中有线程获取到实例了
        if(self.isNull())
        {
            self.reset(new MyLog);
        }
    }
    return self.data();

}

MyLog::MyLog()
{
    //设置日志文件夹的路径，./exe
    logDir.setPath("log");
    //获取日志的绝对路径
    QString logPath = logDir.absoluteFilePath("today.log");

    //获取日志文件创建的时间
    //保存日志文件最后的修改时间
    logFileCreateDate = QFileInfo(logPath).lastModified().date();

    //打开并备份日志文件
    openAndBackupLogFile();

    //每10分钟检查一次日志文件创建的时间
    renameLogFileTimer.setInterval(1000 * 60 *1000);
    renameLogFileTimer.start();

    //处理超时事件，10分钟重复一次
    QObject::connect(&renameLogFileTimer,&QTimer::timeout,[this](){
        QMutexLocker locker(&MyLog::logMutex);
        openAndBackupLogFile();
        checkLogFiles();
        autoDeleteLog();
    });

    //定时刷新日志输出到日志文件，1秒1刷新
    flushLogFileTimer.setInterval(1000);
    flushLogFileTimer.start();

    QObject::connect(&flushLogFileTimer,&QTimer::timeout,[](){
#if LOG
        // 测试不停地写入当前时间到日志文件
        qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
#endif
        //刷新
        QMutexLocker locker(&MyLog::logMutex);
        if(NULL != logOut)
        {
            logOut->flush();
        }
    });
}

MyLog::~MyLog()
{
    if(NULL != logFile)
    {
        logFile->flush();
        logFile->close();
        logOut = NULL;
        logFile = NULL;
    }
}

//打开并备份之前的日志文件
void MyLog::openAndBackupLogFile()
{
    //有可能一天多次打开日志文件，使用追加的方式打开
    //目录不存在，创建目录
    if(!logDir.exists())
    {
        logDir.mkpath(".");
    }
    //log.txt的路径
    QString logPath = logDir.absoluteFilePath("today.log");

    //程序启动的时候，logfile为空
    if(logFile == NULL)
    {
        //创建新的
        logFile = new QFile(logPath);
        //只写，追加的方式打开日志文件
        //成功，创建文本流对象与日志文件关联，向日志文件写内容
        logOut = (logFile->open(QIODevice::WriteOnly | QIODevice::Text |QIODevice::Append)) ? new QTextStream(logFile) : NULL;

        if(logOut != NULL)
        {
            //设置编码格式
            logOut->setCodec("UTF-8");
        }

        //日志文件第一次创建，创建日期无效，设置为修改日期
        if(logFileCreateDate.isNull())
        {
            logFileCreateDate = QDate::currentDate();
        }
    }

    //程序运行的时候，创建日期不是当前日期，更新日期，重命名，备份老的并生成新的log.txt
    if(logFileCreateDate != QDate::currentDate())
    {
        //先刷新缓冲区，确保内容先输出到文件里
        logFile->flush();
        logFile->close();

        //更新日期到备份文件
        QString backUpLogPath = logDir.absoluteFilePath(logFileCreateDate.toString("yyyy-MM-dd.log"));;
        //备份原来的日志
        QFile::copy(logPath,backUpLogPath);
        //删除原来的日志文件
        QFile::remove(logPath);

        //创建新的log.txt，进行更新
        //只写，截断的方式打开日志
        logFile = new QFile(logPath);
        logOut = (logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) ? new QTextStream(logFile) : NULL;
        //更新为修改时间
        logFileCreateDate = QDate::currentDate();
        if(logOut != NULL)
        {
            logOut->setCodec("UTF-8");
        }
    }
}

//检查文件大小
void MyLog::checkLogFiles()
{
    //日志文件大小超过5m，备份并重新创建日志文件
    if(logFile->size() > 1024* g_logLimitSize)
    {
        //清空缓冲
        logFile->flush();
        logFile->close();

        QString logPath = logDir.absoluteFilePath("today.log");
        //备份老的日志文件
        QString backUplogPath = logDir.absoluteFilePath(logFileCreateDate.toString("yyyy-MM-dd.log"));
        QFile::copy(logPath,backUplogPath);
        QFile::remove(logPath);

        //创建新的日志文件
        logFile = new QFile(logPath);
        logOut = (logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) ? new QTextStream(logFile) :NULL;
        logFileCreateDate = QDate::currentDate();
        if(logOut != NULL)
        {
            logOut->setCodec("UTF-8");
        }
    }
}

//自动删除超过时间的日志文件
void MyLog::autoDeleteLog()
{
    //当前时间
    QDateTime now = QDateTime::currentDateTime();

    //基准，30天前
    QDateTime dateTime1 = now.addDays(-30);
    QDateTime dateTime2;

    QString logPath = logDir.absoluteFilePath("today.log");
    //打开日志目录
    QDir dir(logPath);
    //获取目录下的所有文件信息列表
    QFileInfoList fileList = dir.entryInfoList();
    foreach(QFileInfo f, fileList)
    {
        //跳过文件名为空的文件
        if(f.baseName() == "")
        {
            continue;
        }

        //将文件名解析为日期对象
        dateTime2 = QDateTime::fromString(f.baseName(),"yyyy-MM-dd");
        //大于30天，删除
        if(dateTime2 < dateTime1)
        {
            dir.remove(f.absoluteFilePath());
        }
    }
}


//定义消息处理函数
void MyLog::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&MyLog::logMutex);
        QString level;

        switch (type) {
        case QtDebugMsg:
            level = "DEBUG";
            break;
        case QtInfoMsg:
            level = "INFO";
            break;
        case QtWarningMsg:
            level = "WARN";
            break;
        case QtCriticalMsg:
            level = "ERROR";
            break;
        case QtFatalMsg:
            level = "FATAL";
            break;
        default:
            break;
        }

#if defined (Q_OS_WIN)
        QByteArray localMsg = QTextCodec::codecForName("GB2312")->fromUnicode(msg);
#else
        QByteArray localMsg = msg.toLocal8Bit();
#endif

        //输出到控制台
        std::cout << std::string(localMsg) << std::endl;
        if(NULL == MyLog::logOut)
        {
            return;
        }

        //输出到日志文件
        //获取文件名，去掉路径
        QString fileName = context.file;
        int index = fileName.lastIndexOf(QDir::separator());
        fileName = fileName.mid(index + 1);

        //写入日志信息
        (*MyLog::logOut) << QString("%1 - [%2] (%3:%4, %5): %6\n")
                                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                        .arg(level)
                                        .arg(fileName)
                                        .arg(context.line)
                                        .arg(context.function)
                                        .arg(msg);
}

//安装
void MyLog::installMessageHandler()
{
    qInstallMessageHandler(MyLog::messageHandler);
}

//卸载
void MyLog::uninstallMessageHandler()
{
    qInstallMessageHandler(NULL);
}
