#ifndef READTHREAD_H
#define READTHREAD_H

#include <QThread>
#include<QElapsedTimer>
#include<QTime>

class VideoDecode;
class VideoCodec;


class ReadThread : public QThread
{
    Q_OBJECT
public:
    enum PlayState
    {
        play,
        end
    };
public:

    static ReadThread* getInstance();
    explicit ReadThread(QObject *parent = nullptr);
    ~ReadThread() override;

    void setPath(const QString& path);
    void open(const QString& url = QString());//打开视频
    void close();//关闭视频
    const QString& url();//获取打开的视频地址
protected:
    void run() override;
private:
    static QScopedPointer<ReadThread> self;

    VideoCodec* m_videoCodec = NULL;
    VideoDecode* m_videoDecode = NULL;
    QString m_url;//打开的视频地址
    QString m_path;//视频保存路径
    bool m_play = false;



signals:
    void updateImage(const QImage& image);
    void playState(PlayState state);

};

#endif // READTHREAD_H
