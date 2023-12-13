#include "videocodec.h"
#include<QDebug>

extern "C"{
#include "libavcodec/avcodec.h"
#include"libavformat/avformat.h"
#include"libavutil/avutil.h"
#include"libswscale/swscale.h"
#include"libavutil/imgutils.h"
#include"libavdevice/avdevice.h"
}

#define ERROR_LEN 1024  // 异常信息数组长度
#define PRINT_LOG 1


VideoCodec::VideoCodec()
{

}

VideoCodec::~VideoCodec()
{
    close();
}

//open
bool VideoCodec::open(AVCodecContext *codecContext, QPoint point, const QString &fileName)
{
    if(!codecContext||fileName.isEmpty())
    {
#if PRINT_LOG
        qDebug() << "open err";
#endif
        return false;
    }
    //处理输出格式
    int ret = avformat_alloc_output_context2(&m_formatContext,
                                             NULL,
                                             NULL,
                                             fileName.toStdString().data());
    //处理失败
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "处理输出格式失败";
#endif

    }

    //打开输出文件
    ret = avio_open(&m_formatContext->pb,
                    fileName.toStdString().data(),
                    AVIO_FLAG_WRITE);
    //打开失败
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "打开输出文件失败";
#endif
        close();
        showError(ret);
        return false;
    }

    //打开成功，查询编码器
    const AVCodec* codec = avcodec_find_encoder(m_formatContext->oformat->video_codec);
    //查询编码器失败
    if(!codec)
    {
#if PRINT_LOG
        qDebug() << "查询编码器失败";
#endif
        close();
        showError(AVERROR(ENOMEM));
        return false;
    }
#if PRINT_LOG
    qDebug() << codec->id << " " << codec->name;

#endif
    //查询成功， 开始分配
    m_codecContext = avcodec_alloc_context3(codec);
    //分配失败
    if(!m_codecContext)
    {
#if PRINT_LOG
        qDebug() << "分配失败";
#endif

        close();
        showError(AVERROR(ENOMEM));
        return false;
    }

    //分配成功，设置编码器上下文参数
    m_codecContext->width = codecContext->width;
    m_codecContext->height = codecContext->height;
    m_codecContext->pix_fmt = codec->pix_fmts[0];
    m_codecContext->time_base = {point.y(),point.x()};
    m_codecContext->framerate = {point.x(),point.y()};
    m_codecContext->bit_rate = 10000000;//码率
    m_codecContext->gop_size = 12;
    m_codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    //打开编码器
    ret = avcodec_open2(m_codecContext,NULL,NULL);
    //打开编码器失败
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() <<"打开编码器失败";
#endif
        close();
        showError(ret);
        return false;
    }

    //向媒体文件添加新流
    m_videoStream = avformat_new_stream(m_formatContext,NULL);
    if(!m_videoStream)
    {
        close();
        showError(AVERROR(ENOMEM));
        return false;
    }

    //初始化编解码器参数
    ret = avcodec_parameters_from_context(m_videoStream->codecpar,m_codecContext);
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "初始化编解码器参数失败";
#endif
        close();
        showError(ret);
        return false;
    }

    //初始化成功，开始写入文件头
    ret = avformat_write_header(m_formatContext,NULL);
    if(ret < 0)
    {
#if PRINT_LOG
        qDebug() << "写入文件头失败";
#endif

        close();
        showError(ret);
        return false;
    }
    m_writeHeader = true;

    //分配数据包
    m_packet = av_packet_alloc();
    if(!m_packet)
    {
#if PRINT_LOG
        qDebug() << "分配packet失败";
#endif
        close();
        showError(AVERROR(ENOMEM));
        return false;
    }
    m_frame = av_frame_alloc();
    if(!m_frame)
    {
#if PRINT_LOG
        qDebug() << "分配frame失败";
#endif
        close();
        showError(AVERROR(ENOMEM));
        return false;
    }

    m_frame->format = codec->pix_fmts[0];
#if PRINT_LOG
        qDebug() << "开始录制视频";
#endif

        return true;
}

//write
void VideoCodec::write(AVFrame *frame)
{
    QMutexLocker locker(&m_mutex);
    if(!m_packet)
    {
#if PRINT_LOG
        qDebug() << "写入的时候空包";
#endif

        return;
    }
    //同步解码和编码的图像格式
    if(!swsFormat(frame))
    {
        return;
    }
    if(m_frame)
    {
        m_frame->pts = m_index;
        m_index++;
    }

    //将图像传入编码器
    avcodec_send_frame(m_codecContext,m_frame);

    //从编码器读取编码数据
    while(true)
    {
            //从编码器中读取图像帧
            int ret = avcodec_receive_packet(m_codecContext,m_packet);
        if(ret < 0)
        {
#if 0

        qDebug() << "从编码器中读取图像帧失败";
        qDebug() << ret;
#endif
        break;
        }

        //时间基转换
        av_packet_rescale_ts(m_packet,
                             m_codecContext->time_base,
                             m_videoStream->time_base);
        //将数据包写入输出媒体文件
        av_write_frame(m_formatContext,m_packet);
        av_packet_unref(m_packet);
    }

}

//swsFormat
bool VideoCodec::swsFormat(AVFrame *frame)
{
    if(!frame || frame->width <= 0 || frame->height <= 0)
    {
        return false;
    }
    //初始化图像转换上下文
    if(!m_swsContext)
    {
        m_swsContext = sws_getCachedContext(m_swsContext,
                                            frame->width,//输入图像的宽
                                            frame->height,//输入图像的高,
                                            (AVPixelFormat)frame->format,//输入图像的像素格式
                                            frame->width,//输出图像的宽
                                            frame->height,//输出图像的高
                                            (AVPixelFormat)m_frame->format,//输出图像的像素格式
                                            SWS_BILINEAR,//选择缩放算法
                                            NULL,
                                            NULL,
                                            NULL);
        if(!m_swsContext)
        {
#if PRINT_LOG
        qDebug() << "sws_getCachedContext err";
#endif
        av_frame_unref(frame);
        return false;
        }

        if(m_frame)
        {
            //创建图像帧用于保存YUV420P图像
            m_frame->width = frame->width;
            m_frame->height = frame->height;
            av_frame_get_buffer(m_frame,3*8);
        }
    }

    if(m_frame->width <= 0 || m_frame->height <= 0)
    {
#if PRINT_LOG
        qDebug() << "m_frame没有分配空间";
#endif
        return false;
    }
    //开始转换图像像素格式
    bool ret = sws_scale(m_swsContext,//缩放上下文
                         frame->data,//原图像数组
                         frame->linesize,//包含原图像每个平面步幅的数组
                         0,//开始位置
                         frame->height,//行数
                         m_frame->data,//目标图像数组
                         m_frame->linesize//包含目标图像每个平面步幅的数组
            );
    av_frame_unref(frame);
    return ret;
}

//close
void VideoCodec::close()
{
    write(NULL);//传入空帧
    //防止在close的时候write
    QMutexLocker locker(&m_mutex);
    if(m_formatContext)
    {
        //写入文件尾部
        if(m_writeHeader)
        {
            m_writeHeader = false;
            int ret = av_write_trailer(m_formatContext);
            //写入尾部失败
            if(ret < 0)
            {
#if PRINT_LOG
                qDebug() << "写入文件尾部失败";
#endif
                showError(ret);
                return;
            }
        }

        int ret = avio_close(m_formatContext->pb);
        if(ret < 0)
        {
            showError(ret);
            return;
        }
        avformat_free_context(m_formatContext);
        m_formatContext = NULL;
        m_videoStream = NULL;

    }

    //释放编解码器上下文并置空
    if(m_codecContext)
    {
        avcodec_free_context(&m_codecContext);
    }
    if(m_packet)
    {
        av_packet_free(&m_packet);
    }
    if(m_swsContext)
    {
        sws_freeContext(m_swsContext);
        m_swsContext = NULL;
    }

    if(m_frame)
    {
        av_frame_free(&m_frame);
    }
    m_index = 0;
}

//showError
void VideoCodec::showError(int err)
{
    static char m_error[ERROR_LEN];
    memset(m_error,0,ERROR_LEN);
    av_strerror(err,m_error,ERROR_LEN);
    qWarning() << "videosave error: " << m_error;
}
