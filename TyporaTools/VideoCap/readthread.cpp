#include "readthread.h"
#include"videocodec.h"
#include"videodecode.h"

#include<QEventLoop>
#include<QTimer>
#include<QDebug>
#include<qimage.h>
#include<QFileInfo>
#include<QMessageBox>
#include<QDebug>

#define LOG 1

QScopedPointer<ReadThread> ReadThread::self;

//定义单例模式，确保录制线程只能有一个
ReadThread* ReadThread::getInstance()
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
            self.reset(new ReadThread);
        }
    }
    return self.data();

}


ReadThread::ReadThread(QObject *parent) : QThread(parent)
{
    m_videoCodec = new VideoCodec();
    m_videoDecode = new VideoDecode();

    qRegisterMetaType<PlayState>("PlayState");
}

ReadThread::~ReadThread()
{
    if(m_videoDecode)
    {
        delete m_videoDecode;
    }
}

void ReadThread::setPath(const QString &path)
{
    if(path.isEmpty())
    {
#if LOG
    qDebug() << "地址空的";
#endif
        return;
    }

    if(QFileInfo(path).suffix().isEmpty())
    {
        QMessageBox::warning(NULL,
                             "注意",
                             "输入文件没有后缀名");
        m_path.clear();
        return;
    }
#if LOG
    qDebug() << path;
#endif
    m_path = path;

}

void ReadThread::open(const QString &url)
{
    if(!this->isRunning())
    {
        m_url = url;
        emit this->start();
    }
}

void ReadThread::close()
{
    m_play = false;
}

const QString& ReadThread::url()
{
    return m_url;
}

void sleepMsec(int msec)
{
    if(msec <= 0)
    {
        return;
    }
    QEventLoop loop;
    QTimer::singleShot(msec,
                       &loop,
                       SLOT(quit()));
    loop.exec();
}

void ReadThread::run()
{
    if(m_path.isEmpty())
    {
#if LOG
        qDebug() << "地址是空的";
#endif
        return;
    }
    bool ret = m_videoDecode->open(m_url);
    if(ret)
    {
        m_play = m_videoCodec->open(m_videoDecode->getCodecContext(),
                                    m_videoDecode->avgFrameRate(),
                                    m_path);
        if(!m_play)
        {
            qDebug() << "打开输出文件失败";
        }
        else
        {
            emit playState(play);
        }
    }
    else
    {
        qWarning() << "打开失败";
    }
    //读取图像
    while(m_play)
    {
        AVFrame* frame = m_videoDecode->read();
        if(frame)
        {
            m_videoCodec->write(frame);
        }
        else
        {
            if(m_videoDecode->isEnd())
            {
                break;
            }
            sleepMsec(1);
        }
    }
    qDebug() << "录屏结束";
    m_videoDecode->close();
    m_videoCodec->close();
    emit playState(end);
}
