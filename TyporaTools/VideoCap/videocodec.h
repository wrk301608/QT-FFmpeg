#ifndef VIDEOCODEC_H
#define VIDEOCODEC_H
#include<QPoint>
#include<qmutex.h>
#include<qstring.h>

//编码

struct AVCodecParameters;
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;
struct AVPacket;
struct AVOutputFormat;
struct SwsContext;

class VideoCodec
{
public:
    VideoCodec();
    ~VideoCodec();

    bool open(AVCodecContext* codecContext,
              QPoint point,
              const QString& fileName);
    void write(AVFrame* frame);
    void close();
private:
    void showError(int err);
    bool swsFormat(AVFrame* frame);

private:
    AVFormatContext* m_formatContext = NULL;
    AVCodecContext* m_codecContext = NULL;
    SwsContext* m_swsContext = NULL;
    AVStream* m_videoStream = NULL;
    AVPacket* m_packet = NULL;
    AVFrame* m_frame = NULL;
    int m_index = 0;
    bool m_writeHeader = false;
    QMutex m_mutex;
};

#endif // VIDEOCODEC_H
