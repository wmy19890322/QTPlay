#include "VideoWidget.h"
#include <QPainter>
#include <QMessageBox>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include "ThreadAudio.h"

VideoWidget::VideoWidget(QWidget *parent /*= NULL*/) : QOpenGLWidget(parent)
{
    m_pVideoData = NULL;
    m_pFFMpegControl = NULL;
}
void VideoWidget::initializeGL()
{

}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::paintEvent(QPaintEvent *)
{
    if( m_pVideoData != NULL )
    {
        QImage img(m_pVideoData, m_rcVideoShow.width(), m_rcVideoShow.height(), QImage::Format_ARGB32);
        QPainter painter;
        painter.begin(this);
        painter.drawImage(QPoint(m_rcVideoShow.left(), m_rcVideoShow.top()), img);
        painter.end();
    }

    return;
}

void VideoWidget::timerEvent(QTimerEvent *)
{
    this->update();//定时器更新
}

// 播放
int64_t VideoWidget::Play(const char * fileName)
{
    m_pFFMpegControl = new FFMpegControl();
    connect(m_pFFMpegControl, SIGNAL(SendVideoData(uchar *)), this, SLOT(VideoData(uchar *)));
    connect(m_pFFMpegControl, SIGNAL(SendTimeData(int64_t)), this, SLOT(TimeData(int64_t)));
    int64_t time = m_pFFMpegControl->Play(fileName, this->size(), &m_rcVideoShow);
    m_pFFMpegControl->start();
    startTimer(1);
    return time;
}

// 偏移视频
void VideoWidget::Seek(float fPos)
{
    if(m_pFFMpegControl != NULL)
    {
        m_pFFMpegControl->Seek(fPos);
    }
}

// 视频信息
void VideoWidget::VideoData(uchar * data)
{
    m_pVideoData = data;
}

// 进度信息
void VideoWidget::TimeData(int64_t time)
{
    emit SendTimeData(time);
}
