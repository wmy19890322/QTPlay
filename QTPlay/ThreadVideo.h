#ifndef THREADVIDEO_H
#define THREADVIDEO_H

#include <QThread>
#include <queue>
#include "Dexter.h"

// 视频线程
class ThreadVideo : public QThread
{
    Q_OBJECT

    // 线程变量
public:
    // 互斥锁
    QMutex              m_MutexVideo;
    QMutex              m_MutexAVPacket;

    // 视频变量
public:
    QRect               m_rcVideoShow;                  // 显示窗口
    int                 m_nVideoIndex;
    AVCodecContext *    m_pVideoAcodecCtx;
    AVFrame *           m_pVideoFrame;
    AVFrame *           m_pVideoFrameRGB;
    unsigned char *     m_pVideoBuffer;
    SwsContext *        m_pVideoSwsContext;
    double              m_dVideoFPS;
    AVRational          m_AVRational;
    queue<AVPacket *>   m_queAVPacket;

    // 类函数
public:
    // 构造函数
    explicit ThreadVideo(QObject *parent = nullptr);
    // 析构函数
    ~ThreadVideo();

    // 操作函数
public:
    // 初始化
    bool Init(AVFormatContext * pFormatCtx, const QSize szWindow, QRect * prcVideoShow);
    // 设置帧数包
    void AppendPacket(AVPacket * pPacket);
    // 获取视频索引
    inline int GetVideoIndex() { return m_nVideoIndex; }
    // 删除缓存
    void FlushBuffer();

    // 通知消息
signals:
    // 发送视频数据
    void SendVideoData(uchar * data);
    // 发送进度信息
    void SendTimeData(int64_t time);

    // 线程函数
public:
    // 线程执行
    virtual void run();

};

#endif // THREADVIDEO_H
