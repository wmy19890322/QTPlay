#include "FFMpegControl.h"

FFMpegControl::FFMpegControl()
{
    // 通用变量
    m_pFormatCtx = NULL;


    m_pThreadVideo = new ThreadVideo(this);
    m_pThreadAudio = new ThreadAudio(this);
    connect(m_pThreadVideo, SIGNAL(SendVideoData(uchar*)), this, SLOT(AcceptVideoData(uchar*)));
    connect(m_pThreadVideo, SIGNAL(SendTimeData(int64_t)), this, SLOT(AcceptTimeData(int64_t)));
    connect(m_pThreadAudio, SIGNAL(SendTimeData(int64_t)), this, SLOT(AcceptTimeData(int64_t)));
}

FFMpegControl::~FFMpegControl()
{
    avformat_close_input(&m_pFormatCtx);
    avformat_free_context(m_pFormatCtx);
}

// 播放文件
int64_t FFMpegControl::Play(const char * fileName, const QSize szWindow, QRect * prcVideoShow)
{
    // 初始化帧数结构
    m_pFormatCtx = avformat_alloc_context();

    // 打开音视频文件并初始化 AVFormatContext 结构体
    if (avformat_open_input(&m_pFormatCtx, fileName, NULL, NULL) != 0)
    {
        DebugError("avformat_open_input");
        return 0;
    }

    // 根据AVFormatContext结构体,来获取视频上下文信息,并初始化streams[]成员
    if (avformat_find_stream_info(m_pFormatCtx, NULL) != 0)
    {
        DebugError("avformat_find_stream_info");
        return 0;
    }

    // 音视频初始化 ------------------------------------------------------------------
    m_pThreadVideo->Init(m_pFormatCtx, szWindow, prcVideoShow);
    m_pThreadAudio->Init(m_pFormatCtx);

    // 开始解析
    m_pThreadVideo->start();
    m_pThreadAudio->start();

    return m_pFormatCtx->duration;
}

// 读取包
AVPacket * FFMpegControl::Read()
{
    // 互斥锁
    DMutex dMutex(m_MutexFFMpeg);

    // 申请解析包控件
    AVPacket * pPacket = av_packet_alloc();

    // 读取帧信息
    if( av_read_frame(m_pFormatCtx, pPacket) < 0 )
    {
        av_packet_free(&pPacket);

        return NULL;
    }

    return pPacket;
}

// 偏移视频
void FFMpegControl::Seek(float fPos)
{
    // 互斥锁
    DMutex dMutex(m_MutexFFMpeg);

    // 偏移视频
    int nVideoIndex = m_pThreadVideo->GetVideoIndex();
    if( nVideoIndex != -1 )
    {
        int64_t stamp = fPos * m_pFormatCtx->streams[nVideoIndex]->duration;

        // 将视频移至到当前点击滑动条位置
        av_seek_frame(m_pFormatCtx, nVideoIndex, stamp, AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);

        // 刷新缓冲,清理掉
        m_pThreadVideo->FlushBuffer();
    }

    // 偏移音频
    int nAudioIndex = m_pThreadAudio->GetAudioIndex();
    if( nAudioIndex != -1 )
    {
        int64_t stamp = fPos * m_pFormatCtx->streams[nAudioIndex]->duration;

        // 将音频移至到当前点击滑动条位置
        av_seek_frame(m_pFormatCtx, nAudioIndex, stamp, AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);

        // 刷新缓冲,清理掉
        m_pThreadAudio->FlushBuffer();
    }
}

// 视频信息
void FFMpegControl::AcceptVideoData(uchar * data)
{
    // 给窗口发送视频数据
    emit SendVideoData(data);
}

// 进度信息
void FFMpegControl::AcceptTimeData(int64_t time)
{
    // 给窗口发送进度信息
    emit SendTimeData(time);
}

// 线程执行
void FFMpegControl::run()
{
    while( true )
    {
        // 读取帧数
        AVPacket * pPacket = Read();

        // 无效帧数
        if( pPacket == NULL )
        {
            continue;
        }

        // 解码帧数
        if (pPacket->stream_index == m_pThreadAudio->GetAudioIndex())
        {
            m_pThreadAudio->AppendPacket(pPacket);
        }
        else if( pPacket->stream_index == m_pThreadVideo->GetVideoIndex() )
        {
            m_pThreadVideo->AppendPacket(pPacket);
        }
        else
        {
            av_packet_free(&pPacket);
        }

    };
}
