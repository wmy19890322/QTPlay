#ifndef THREADAUDIO_H
#define THREADAUDIO_H

#include <QThread>
#include <QAudioOutput>
#include "Dexter.h"

class ThreadAudio : public QThread
{
    Q_OBJECT

    // 线程变量
public:
    // 互斥锁
    QMutex              m_MutexAudio;
    QMutex              m_MutexAVPacket;

    // 音频变量
public:
    int                 m_nAudioIndex;
    AVCodecContext *    m_pAudioAcodecCtx;
    AVFrame *           m_pAudioFrame;
    SwrContext *        m_pAudioSwrContext;
    QAudioOutput *      m_pAudioOutput;
    QIODevice*          m_pIODevice;
    queue<AVPacket *>   m_queAVPacket;
    bool                m_bFlushBuffer;

    // 类函数
public:
    // 构造函数
    explicit ThreadAudio(QObject *parent = nullptr);
    // 析构函数
    ~ThreadAudio();

    // 操作函数
public:
    // 初始化
    bool Init(AVFormatContext * pFormatCtx);
    // 设置帧数包
    void AppendPacket(AVPacket * pPacket);
    // 获取视频索引
    inline int GetAudioIndex() { return m_nAudioIndex; }
    // 删除缓存
    void FlushBuffer();

    // 通知消息
signals:
    // 发送进度信息
    void SendTimeData(int64_t time);

    // 线程函数
public:
    // 线程执行
    virtual void run();

};

#endif // THREADAUDIO_H
