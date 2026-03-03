#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "libavcodec/packet.h"
#include <DemuxThread.h>
#include <QObject>
#include <SafeQueue.h>
#include <VideoDecoder.h>
#include <displayWidget.h>


class PlayerController : public QObject{
    Q_OBJECT
public:
    PlayerController(std::string name,displayWidget *disWidget){
        videoPacketQueue = new SafeQueue<AVPacket*>();
        studioPacketQueue = new SafeQueue<AVPacket*>();
        demuxer = new DemuxThread(videoPacketQueue,name);
        decoder = new VideoDecoder(videoPacketQueue,demuxer->getCodeCtx());
        connect(decoder,&VideoDecoder::frameReady,disWidget,&displayWidget::uploadYuv);
    }

    void play(){
        demuxer->start();
        decoder->start();
    }

    ~PlayerController() {
        if (demuxer) {
            demuxer->requestInterruption();
            demuxer->wait();
            delete demuxer;
        }
        if (decoder) {
            decoder->wait();
            delete decoder;
        }
        // 清空队列并释放
        if (videoPacketQueue) {
            delete videoPacketQueue;
        }
    }

private:
    SafeQueue<AVPacket*> *videoPacketQueue;
    SafeQueue<AVPacket*> *studioPacketQueue;
    DemuxThread *demuxer;
    VideoDecoder *decoder;
};

#endif // PLAYERCONTROLLER_H
