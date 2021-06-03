#include "ThreadVideo.h"
#include <QDateTime>

ThreadVideo::ThreadVideo(QObject *parent) : QThread(parent)
{
    // 视频变量
    m_nVideoIndex = -1;
    m_pVideoAcodecCtx = NULL;
    m_pVideoFrame = NULL;
    m_pVideoFrameRGB = NULL;
    m_pVideoBuffer = NULL;
    m_pVideoSwsContext = NULL;
    m_dVideoFPS = 0.0;
}

// 析构函数
ThreadVideo::~ThreadVideo()
{
    av_frame_free(&m_pVideoFrame);
    av_frame_free(&m_pVideoFrameRGB);
    av_free(m_pVideoBuffer);
    sws_freeContext(m_pVideoSwsContext);
    m_pVideoSwsContext = NULL;
    avcodec_close(m_pVideoAcodecCtx);
}

// 初始化
bool ThreadVideo::Init(AVFormatContext * pFormatCtx, const QSize szWindow, QRect * prcVideoShow)
{
    // 视频初始化 ------------------------------------------------------------------

    // 找到视频流
    m_nVideoIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (m_nVideoIndex == -1)
    {
        DebugError("no find video stream");
        return false;
    }

    // 获取视频解析控制
    m_pVideoAcodecCtx = avcodec_alloc_context3(avcodec_find_decoder(pFormatCtx->streams[m_nVideoIndex]->codecpar->codec_id));
    avcodec_parameters_to_context(m_pVideoAcodecCtx, pFormatCtx->streams[m_nVideoIndex]->codecpar);
    if (avcodec_open2(m_pVideoAcodecCtx, NULL,NULL) != 0)
    {
        DebugError("no find video avcodec");
        m_nVideoIndex = -1;
        return false;
    }

    m_dVideoFPS = r2d(pFormatCtx->streams[m_nVideoIndex]->avg_frame_rate);
    m_AVRational = pFormatCtx->streams[m_nVideoIndex]->time_base;

    // 格式合适的窗口
    double dx = (double)szWindow.width() / (double)m_pVideoAcodecCtx->width;
    double dy = (double)szWindow.height() / (double)m_pVideoAcodecCtx->height;
    if( dx < dy )
    {
        prcVideoShow->setRect(0, szWindow.height() / 2 - (m_pVideoAcodecCtx->height * dx / 2), szWindow.width(), m_pVideoAcodecCtx->height * dx );
    }
    else
    {
        prcVideoShow->setRect(szWindow.width() / 2 - (m_pVideoAcodecCtx->width * dy / 2), 0, m_pVideoAcodecCtx->width * dy, szWindow.height() );
    }
    m_rcVideoShow = *prcVideoShow;

    // 申请视频帧的空间
    m_pVideoFrame = av_frame_alloc();
    m_pVideoFrameRGB = av_frame_alloc();

    // 创建动态内存,创建存储图像数据的空间
    // av_image_get_buffer_size():根据像素格式、图像宽、图像高来获取一帧图像需要的大小(第4个参数align:表示多少字节对齐,一般填1,表示以1字节为单位)
    // av_malloc():给out_buffer分配一帧RGB32图像显示的大小
    m_pVideoBuffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, m_rcVideoShow.width(), m_rcVideoShow.height(), 1));

    // 通过av_image_fill_arrays和out_buffer来初始化pFrameRGB里的data指针和linesize指针.linesize是每个图像的宽大小(字节数)。
    av_image_fill_arrays(m_pVideoFrameRGB->data, m_pVideoFrameRGB->linesize, m_pVideoBuffer, AV_PIX_FMT_RGB32, m_rcVideoShow.width(), m_rcVideoShow.height(), 1);

    // 初始化SwsContext结构体,设置像素转换格式规则,将pCodecCtx->pix_fmt格式转换为AV_PIX_FMT_RGB32格式
    m_pVideoSwsContext = sws_getContext(m_pVideoAcodecCtx->width, m_pVideoAcodecCtx->height, m_pVideoAcodecCtx->pix_fmt, m_rcVideoShow.width(), m_rcVideoShow.height(), AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    return true;
}

// 设置帧数包
void ThreadVideo::AppendPacket(AVPacket * pPacket)
{
    // 队列消息
    DMutex dMutex(m_MutexAVPacket);

    // 添加消息
    m_queAVPacket.push(pPacket);
}

// 删除缓存
void ThreadVideo::FlushBuffer()
{
    // 互斥锁
    DMutex dMutexPacket(m_MutexAVPacket);
    DMutex dMutexVideo(m_MutexVideo);

    // 删除解码包数据
    while( m_queAVPacket.size() > 0 ) {
        AVPacket * pPacket = m_queAVPacket.front();
        m_queAVPacket.pop();
        av_packet_free(&pPacket);
    }

    // 清空解码数据
    avcodec_flush_buffers(m_pVideoAcodecCtx);
}

// 线程执行
void ThreadVideo::run()
{

    while( true )
    {
        // 开始时间
        qint64 timeBegin = QDateTime::currentDateTime().toMSecsSinceEpoch();

        // 帧数包
        AVPacket * pPacket = NULL;

        // 取出数据
        m_MutexAVPacket.lock();
        if( m_queAVPacket.size() > 0 ) {
            pPacket = m_queAVPacket.front();
            m_queAVPacket.pop();
        }
        m_MutexAVPacket.unlock();

        // 无效数据
        if(pPacket == NULL)
        {
            continue;
        }

        // 锁定视频
        m_MutexVideo.lock();

        // 解码包
        if (avcodec_send_packet(m_pVideoAcodecCtx, pPacket) != 0
            || avcodec_receive_frame(m_pVideoAcodecCtx, m_pVideoFrame) != 0)
        {
            av_packet_free(&pPacket);
            m_MutexVideo.unlock();
            continue;
        }

        // 进行视频像素格式和分辨率的转换
        sws_scale(m_pVideoSwsContext, (const unsigned char* const*)m_pVideoFrame->data, m_pVideoFrame->linesize, 0, m_pVideoAcodecCtx->height,
                  m_pVideoFrameRGB->data, m_pVideoFrameRGB->linesize);

        // 发送解码数据
        emit SendVideoData((uchar*)m_pVideoFrameRGB->data[0]);

        // 发送播放时间
        int64_t pts = m_pVideoFrame->pts * r2d(m_AVRational) * AV_TIME_BASE;
        emit SendTimeData( pts );

        // 当帧时间
        int64_t timePacket = pPacket->duration * r2d(m_AVRational) * AV_TIME_BASE / 1000;

        // 释放包数据
        av_packet_free(&pPacket);

        // 解锁
        m_MutexVideo.unlock();

        // 控制解码的进度
        int64_t timeSleep = timePacket - (QDateTime::currentDateTime().toMSecsSinceEpoch() - timeBegin);
        if (timeSleep > 0)
        {
            // 换帧时间
            msleep(timeSleep);
        }
    };
}
