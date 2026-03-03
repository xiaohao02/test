#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include <QThread>

extern "C"{
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
}

class VideoThread : public QThread{
    Q_OBJECT
public:
    VideoThread();
    void run();

signals:
    void frameReady(uchar *y,uchar *u,uchar *v,int w,int h);
};

#endif // VIDEOTHREAD_H
