#ifndef DEXTER_H
#define DEXTER_H
#include <QDebug>
#include <QMutex>
#include <QAudioOutput>
#include <QSize>
#include <QRect>
#include <queue>
using namespace std;

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavformat/version.h>
    #include <libavutil/time.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/imgutils.h>
    #include <libswresample/swresample.h>
};

#define _T(str)   (QString::fromLocal8Bit(str).toStdString().c_str())


// 时间转换
inline double r2d(AVRational & r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}


// 微妙转换时间
inline QString TimeToString(int64_t time)
{
    int hour = time / 1000 / 1000 / 60 / 60;
    int minute = (time / 1000 / 1000 / 60) % 60;
    int second = (time / 1000 / 1000) % 60;
    int millisecond = (time / 1000 ) % 1000;
    return QString("%1:%2:%3.%4").arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0')).arg(millisecond, 3, 10, QChar('0'));
}


// 根据错误编号获取错误信息并打印
inline void DebugError(QString prefix, int err = 0)
{
    if( err != 0 )
    {
        char errbuf[512] = {0};
        av_strerror(err, errbuf, sizeof(errbuf));
        qDebug() << prefix << ":" << errbuf;
    }
    else
    {
        qDebug() << prefix;
    }
}

// 线程锁
class DMutex
{
public:
    QMutex & Mutex;
    DMutex( QMutex & MutexpRar ) : Mutex(MutexpRar)
    {
        Mutex.lock();
    }

    ~DMutex()
    {
        Mutex.unlock();
    }
};

#endif // DEXTER_H
