#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#pragma once
#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>
#include <QAudioOutput>
#include "Dexter.h"
#include "FFMpegControl.h"

class VideoWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent = NULL);

    virtual void paintEvent(QPaintEvent *event);//窗口的重新绘制
    virtual void timerEvent(QTimerEvent *event);//定时器
    virtual ~VideoWidget();

virtual void initializeGL();


    // 变量定义
public:
    QRect       m_rcVideoShow;
    uchar *     m_pVideoData;
    FFMpegControl * m_pFFMpegControl;

    // 控制函数
public:
    // 播放
    int64_t Play(const char * fileName);
    // 偏移视频
    void Seek(float fPos);

    // 接受消息
public slots:
    // 视频信息
    void VideoData(uchar * data);
    // 进度信息
    void TimeData(int64_t time);

signals:
    // 给窗口发送进度
    void SendTimeData(int64_t time);
};

#endif // VIDEOWIDGET_H
