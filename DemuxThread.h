#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H

#include "safequeue.h"
#include <QThread>

extern "C"{
    #include<libavformat/avformat.h>
    #include<libavcodec/avcodec.h>
    #include <libavcodec/packet.h>
}

template <typename T>
using FFPtr = std::unique_ptr<T, void(*)(T*)>;

class DemuxThread : public QThread{
public:
    DemuxThread(SafeQueue<AVPacket*> *vq,std::string fileName);
    void run();
    AVCodecContext* getCodeCtx();
private:
    SafeQueue<AVPacket*> *videoQueue;
    SafeQueue<AVPacket*> *audioQueue;
    std::string fileName;
    AVFormatContext *formatCtx;
    AVCodecContext *codeCtx;
    AVCodecParameters *codePara;
    const AVCodec* codec;
    AVPacket *packet;
    int videoStream ;
    bool running ;
};

#endif // DEMUXTHREAD_H
