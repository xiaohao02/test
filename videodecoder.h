#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include<QThread>
#include "safequeue.h"

extern "C"{
    #include<libavformat/avformat.h>
    #include<libavcodec/avcodec.h>
    #include <libavcodec/packet.h>
}

class VideoDecoder : public QThread
{
    Q_OBJECT
public:
    VideoDecoder(SafeQueue<AVPacket*> *vq,AVCodecContext *codeCtx);
    void run();

private:
    SafeQueue<AVPacket*> *videoQueue;
    AVCodecContext *codeCtx;
    AVFrame *frame;
    AVPacket *packet;
    AVFrame *sendFrame;

signals:
    void frameReady(AVFrame *frame);
};

#endif // VIDEODECODER_H
