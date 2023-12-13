#include "videodecode.h"
#include<QDebug>
#include<QImage>
#include<QMutex>
#include<qdatetime.h>

extern "C"{
#include"libavcodec/avcodec.h"
#include"libavformat/avformat.h"
#include"libavutil/avutil.h"
#include"libswscale/swscale.h"
#include"libavutil/imgutils.h"
#include"libavdevice/avdevice.h"
}

#define ERROR_LEN 1024
#define PRINT_LOG 1

VideoDecode::VideoDecode()
{
    //初始化
    initFFmpeg();
    //实例错误信息
    m_error = new char[ERROR_LEN];

    //查询输入设备格式
    m_inputFormat = av_find_input_format("gdigrab");
#if PRINT_LOG
    if(!m_inputFormat)
    {
        qWarning() << "查询输入设备失败";
    }
#endif
}

//初始化FFMpeg,单例
void VideoDecode::initFFmpeg()
{
    static bool isFirst = true;
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if(isFirst)
    {
        //初始化libacdevice并注册所有的输入和输出设备
        avformat_network_init();
        avdevice_register_all();
        isFirst = false;
    }
}
//打开视频流
bool VideoDecode::open(const QString &url)
{
    if(url.isNull())
    {
#if PRINT_LOG
        qDebug() << "open url is null";
#endif
        return false;
    }

    //创建参数字典
    AVDictionary* dict = NULL;
    //设置参数
    av_dict_set(&dict,
                "framerate",//帧率
                "40",
                0);
    //包含鼠标
    av_dict_set(&dict,
                "draw_mouse",
                "1",
                0);
    //录制视频的宽高
    av_dict_set(&dict,
                "video_size",
                "1920x1080",
                0);
    //录制视频的起点
    //打开输入文件流，并返回解封装上下文
    int ret = avformat_open_input(&m_formatContext,//保存解封装上下文
                                  url.toStdString().data(),//打开视频的地址
                                  m_inputFormat,
                                  &dict);//设置参数
    //防止内存泄漏，手动释放参数字典
    if(dict)
    {
        av_dict_free(&dict);
    }
    //处理打开视频失败
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "decode open video err";
#endif
        showError(ret);
        free();
        return false;
    }

    //打开成功
    //读取媒体文件的数据包获取流信息
    ret = avformat_find_stream_info(m_formatContext,
                                    NULL);
    //读取失败
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "read stream err";
#endif
        showError(ret);
        free();
        return false;
    }
    //读取成功
    //计算视频总时长,毫秒
    m_totalTime = m_formatContext->duration / (AV_TIME_BASE / 1000);
#if PRINT_LOG
    qDebug() << QString("视频总时长：%1 ms,[%2]")
                .arg(m_totalTime)
                .arg(QTime::fromMSecsSinceStartOfDay(int(m_totalTime)).toString("HH:mm:ss zzz"));
#endif

    //查询视频流ID
    m_videoIndex = av_find_best_stream(m_formatContext,
                                       AVMEDIA_TYPE_VIDEO,
                                       -1,
                                       -1,
                                       NULL,
                                       0);
    //查找失败
    if(m_videoIndex < 0)
    {
#if PRINT_LOG
        qDebug() << "video Index find err";
#endif
        showError(m_videoIndex);
        free();
        return false;
    }

    //查找成功
    //通过索引值获取视频流
    AVStream* videoStream = m_formatContext->streams[m_videoIndex];

    //获取视频图像分辨率
    m_size.setWidth(videoStream->codecpar->width);
    m_size.setHeight(videoStream->codecpar->height);
    m_frameRate = rationalToDouble(&videoStream->avg_frame_rate);
    m_avgFrameRate.setX(videoStream->avg_frame_rate.num);
    m_avgFrameRate.setY(videoStream->avg_frame_rate.den);

    //获取视频解码器
    const AVCodec* codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    m_totalFrames = videoStream->nb_frames;

#if PRINT_LOG
    qDebug() << QString("分辨率：[w:%1,h:%2] 帧率:%3 总帧数:%4 解码器:%5")
                .arg(m_size.width())
                .arg(m_size.height())
                .arg(m_frameRate)
                .arg(m_totalFrames)
                .arg(codec->name);

#endif

    //获取解码器上下文
    m_codecContext = avcodec_alloc_context3(codec);
    if(!m_codecContext)
    {
#if PRINT_LOG
        qWarning() << "创建解码器上下文失败";
#endif
        free();
        return false;
    }

    //给解码器上下文赋值
    ret = avcodec_parameters_to_context(m_codecContext,
                                        videoStream->codecpar);

    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "给解码器上下文赋值失败";
#endif
        showError(ret);
        free();
        return false;
    }

    m_codecContext->flags2 |= AV_CODEC_FLAG2_FAST;    // 允许不符合规范的加速技巧。
    m_codecContext->thread_count = 8;                 // 使用8线程解码

    //打开解码器
    ret = avcodec_open2(m_codecContext,
                        NULL,
                        NULL);
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "打开解码器失败";
#endif
        showError(ret);
        free();
        return false;
    }

    //分配AVPacket字段
    m_packet = av_packet_alloc();
    if(!m_packet)
    {
#if PRINT_LOG
        qWarning() << "av_packet_molloc err";
#endif
        free();
        return false;
    }

    //分配AVFrame
    m_frame = av_frame_alloc();
    if(!m_frame)
    {
#if PRINT_LOG
        qWarning() << "av_frame_alloc err";
#endif
        free();
        return false;
    }

    m_end = false;
    //open成功
    return true;
}

//read
AVFrame* VideoDecode::read()
{
    //开始读取数据帧
    //如果打开视频失败就返回
    if(!m_formatContext)
    {
#if PRINT_LOG
        qDebug() << "读取视频的时候打开失败";
#endif
        return NULL;
    }

    //成功打开，开始读取数据
    int readRet = av_read_frame(m_formatContext,m_packet);

    if(readRet < 0)
    {
        avcodec_send_packet(m_codecContext,m_packet);
    }

    else
    {
        //是图像数据，进行解码
        if(m_packet->stream_index == m_videoIndex)
        {
            //把读取到的原始数据包发给解码器进行解码
            int ret = avcodec_send_packet(m_codecContext,m_packet);
            if(ret < 0)
            {
#if PRINT_LOG
                qDebug() << "原始数据包发给解码器失败";
#endif
                showError(ret);
            }
        }
    }

    //释放数据包
    av_packet_unref(m_packet);
    av_frame_unref(m_frame);

    //从解码器读取解码后的数据
    int ret = avcodec_receive_frame(m_codecContext,m_frame);
    //读取失败
    if(ret < 0)
    {
        av_frame_unref(m_frame);
        if(readRet < 0)
        {
            m_end = true;
            //读不到数据，而且解码器也没有数据的时候是读取完了
        }
        return NULL;
    }
    //返回读取到的解码完的数据
    return m_frame;

}
//close
void VideoDecode::close()
{
    clear();
    free();

    m_totalTime = 0;
    m_videoIndex = 0;
    m_totalFrames = 0;
    m_obtainFrames = 0;
    m_frameRate = 0;
    m_size = QSize(0,0);
}

//isEnd
bool VideoDecode::isEnd()
{
    return m_end;
}

//clear
void VideoDecode::clear()
{
    if(m_formatContext && m_formatContext->pb)
    {
        avio_flush(m_formatContext->pb);
    }
    if(m_formatContext)
    {
        avformat_flush(m_formatContext);
    }
}

//帧率计算
qreal VideoDecode::rationalToDouble(AVRational *rational)
{
    qreal frameRate = (rational->den == 0) ? 0:(qreal(rational->num)/rational->den);
    return frameRate;
}

//错误处理
void VideoDecode::showError(int err)
{
#if PRINT_LOG
    memset(m_error,0,ERROR_LEN);
    av_strerror(err,m_error,ERROR_LEN);
    qWarning() << "DecodeVideo Error: " << m_error;
#endif
}
//释放资源
void VideoDecode::free()
{
    //释放编解码器上下文
    if(m_codecContext)
    {
        avcodec_free_context(&m_codecContext);
    }
    if(m_formatContext)
    {
        avformat_close_input(&m_formatContext);
    }
    if(m_packet)
    {
        av_packet_free(&m_packet);
    }
    if(m_frame)
    {
        av_frame_free(&m_frame);
    }
}


VideoDecode::~VideoDecode()
{
    close();
}
