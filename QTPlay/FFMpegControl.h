#ifndef FFMPEGCONTROL_H
#define FFMPEGCONTROL_H
#include <QThread>
#include "Dexter.h"
#include "ThreadVideo.h"
#include "ThreadAudio.h"

class FFMpegControl : public QThread
{
    Q_OBJECT

    // 线程变量
public:
    // 互斥锁
    QMutex              m_MutexFFMpeg;

    // 通用变量
public:
    AVFormatContext *   m_pFormatCtx;                   // 帧数控制
    QRect               m_rcVideoShow;                  // 显示窗口

    // 视频线程
public:
    ThreadVideo *       m_pThreadVideo;
    ThreadAudio *       m_pThreadAudio;

public:
    FFMpegControl();
    ~FFMpegControl();

    // 操作函数
public:
    // 播放文件
    int64_t Play(const char * fileName, const QSize szWindow, QRect * prcVideoShow);
    // 读取包
    AVPacket * Read();
    // 偏移视频
    void Seek(float fPos);

    // 通知消息
signals:
    // 发送视频数据
    void SendVideoData(uchar * data);
    // 发送进度信息
    void SendTimeData(int64_t time);


    // 接收数据
public slots:
    // 视频信息
    void AcceptVideoData(uchar * data);
    // 进度信息
    void AcceptTimeData(int64_t time);

    // 线程函数
public:
    // 线程执行
    virtual void run();

};

#endif // FFMPEGCONTROL_H
