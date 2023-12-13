#ifndef VIDEODECODE_H
#define VIDEODECODE_H

//解码类

#include<QString>
#include<QSize>
#include<qfile.h>
#include<QPoint>

struct AVFormatContext;
struct AVCodecContext;
struct AVRational;
struct AVPacket;
struct AVFrame;
struct SwsContext;
struct AVBufferRef;
struct AVInputFormat;
struct AVStream;

class QImage;

class VideoDecode
{
public:
    VideoDecode();
    ~VideoDecode();
    bool open(const QString& url = QString());//打开媒体文件
    AVFrame* read();//读取视频图像
    void close();//关闭
    bool isEnd();//是否读取完成
    AVCodecContext* getCodecContext()
    {
        return m_codecContext;
    }
    QPoint avgFrameRate()
    {
        return m_avgFrameRate;
    }

private:
    void initFFmpeg();//初始化
    void showError(int err);//显示FFmpeg执行错误
    qreal rationalToDouble(AVRational* rational);
    void clear();//清空读取缓冲
    void free();//释放资源

private:
    const AVInputFormat* m_inputFormat = NULL;
    AVFormatContext* m_formatContext = NULL;//解封装上下文
    AVCodecContext* m_codecContext = NULL;//解码器上下文
    AVPacket* m_packet = NULL;//数据包
    AVFrame* m_frame = NULL;//解码后的数据帧
    int m_videoIndex = 0;//视频流索引
    qint64 m_totalTime = 0;//视频总时长
    qint64 m_totalFrames = 0;//视频总帧数
    qint64 m_obtainFrames = 0;//视频当前获取到的帧数
    qreal m_frameRate = 0;//视频帧率
    QSize m_size;//视频分辨率
    char* m_error = NULL;//保存异常信息
    bool m_end = false;//视频读取完成
    QPoint m_avgFrameRate;
};

#endif // VIDEODECODE_H
