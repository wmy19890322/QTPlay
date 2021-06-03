#include "ThreadAudio.h"

// 构造函数
ThreadAudio::ThreadAudio(QObject *parent) : QThread(parent)
{
    // 音频变量
    m_nAudioIndex = -1;
    m_pAudioAcodecCtx = NULL;
    m_pAudioFrame = NULL;
    m_pAudioSwrContext = NULL;
    m_pAudioOutput = NULL;
    m_pIODevice = NULL;
    m_bFlushBuffer = false;
}

// 析构函数
ThreadAudio::~ThreadAudio()
{
    av_frame_free(&m_pAudioFrame);
    swr_free(&m_pAudioSwrContext);
    avcodec_close(m_pAudioAcodecCtx);
    m_pIODevice->close();
    delete m_pAudioOutput;
}

// 初始化
bool ThreadAudio::Init(AVFormatContext * pFormatCtx)
{
    // 视频初始化 ------------------------------------------------------------------

    // 找到音频流
    m_nAudioIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    // 获取解析控制
    m_pAudioAcodecCtx = avcodec_alloc_context3(avcodec_find_decoder(pFormatCtx->streams[m_nAudioIndex]->codecpar->codec_id));
    avcodec_parameters_to_context(m_pAudioAcodecCtx, pFormatCtx->streams[m_nAudioIndex]->codecpar);

    // 打开解码器
    if (avcodec_open2(m_pAudioAcodecCtx, NULL,NULL) != 0)
    {
        DebugError("no find audio avcodec");
        m_nAudioIndex = -1;
        return false;
    }

    // 目标格式
    m_pAudioSwrContext = swr_alloc();
    m_pAudioSwrContext = swr_alloc_set_opts(m_pAudioSwrContext, m_pAudioAcodecCtx->channel_layout, AV_SAMPLE_FMT_S16, m_pAudioAcodecCtx->sample_rate, m_pAudioAcodecCtx->channel_layout, m_pAudioAcodecCtx->sample_fmt, m_pAudioAcodecCtx->sample_rate, 0, NULL);
    swr_init(m_pAudioSwrContext);

    // 申请音频帧空间
    m_pAudioFrame =av_frame_alloc();

    // 启动音频播放器
    QAudioFormat format;
    format.setSampleRate(m_pAudioAcodecCtx->sample_rate);                                   // 设置采样率
    format.setSampleSize( m_pAudioAcodecCtx->sample_fmt == AV_SAMPLE_FMT_S32 ? 32 : 16 );   // 样本数据
    format.setChannelCount(m_pAudioAcodecCtx->channels);                                    // 设置通道数
    format.setCodec("audio/pcm");                                                           // 播出格式为pcm格式
    format.setByteOrder(QAudioFormat::LittleEndian);                                        // 默认小端模式
    format.setSampleType(QAudioFormat::UnSignedInt);                                        // 无符号整形数
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());                         // 选择默认输出设备
    if ( !info.isFormatSupported(format) )
    {
        DebugError("no create audio device");
        m_nAudioIndex = -1;
        return false;
    }
    m_pAudioOutput = new QAudioOutput(format, this);
    m_pAudioOutput->setBufferSize( m_pAudioAcodecCtx->sample_rate * 2 * 2 * 10 );
    m_pAudioOutput->stop();
    m_pIODevice = m_pAudioOutput->start();

    return true;
}

// 设置帧数包
void ThreadAudio::AppendPacket(AVPacket * pPacket)
{
    // 队列消息
    DMutex dMutex(m_MutexAVPacket);

    // 添加消息
    m_queAVPacket.push(pPacket);
}

// 删除缓存
void ThreadAudio::FlushBuffer()
{
    // 互斥锁
    DMutex dMutexPacket(m_MutexAVPacket);
    DMutex dMutexAudio(m_MutexAudio);
    m_bFlushBuffer = true;

    // 删除解码包数据
    while( m_queAVPacket.size() > 0 ) {
        AVPacket * pPacket = m_queAVPacket.front();
        m_queAVPacket.pop();
        av_packet_free(&pPacket);
    }

    // 清空解码数据
    avcodec_flush_buffers(m_pAudioAcodecCtx);
    m_pIODevice->atEnd();
}

// 运行线程
void ThreadAudio::run()
{
    while( true )
    {
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

        // 锁定音频
        DMutex dMutex(m_MutexAudio);

        // 完成释放
        m_bFlushBuffer = false;

        // 解码包
        if (avcodec_send_packet(m_pAudioAcodecCtx, pPacket) != 0
            || avcodec_receive_frame(m_pAudioAcodecCtx, m_pAudioFrame) != 0)
        {
            av_packet_free(&pPacket);
            continue;
        }

        // 解码缓冲区
        uint8_t * data[2] = { 0 };
        int byteCnt = m_pAudioFrame->nb_samples * 2 * 2;

        // frame->nb_samples*2*2表示分配样本数据量*两通道*每通道2字节大小
        uint8_t * pcm = new uint8_t[byteCnt];

        // 输出格式为AV_SAMPLE_FMT_S16(packet类型),所以转换后的LR两通道都存在data[0]中
        data[0] = pcm;

        swr_convert(m_pAudioSwrContext,
                          data, m_pAudioFrame->nb_samples,                                    // 输出
                          (const uint8_t**)m_pAudioFrame->data, m_pAudioFrame->nb_samples );  // 输入

        // 缓冲区已满，等待
        while (m_pAudioOutput->bytesFree() < byteCnt && !m_bFlushBuffer)
        {
            msleep(10);
        }

        if(m_bFlushBuffer)
        {
            // 释放包数据
            av_packet_free(&pPacket);
            continue;
        }

        // 播放音频
        m_pIODevice->write((const char *)pcm, byteCnt);
        delete[] pcm;

        // 释放包数据
        av_packet_free(&pPacket);
    };
}
