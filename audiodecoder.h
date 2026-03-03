#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include<QThread>
#include <SafeQueue.h>

extern "C"{
#include<libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

class AudioDecoder:public QThread
{
    Q_OBJECT
public:
    AudioDecoder(SafeQueue<AVPacket*> *que,AVCodecContext *ctx);
    void initSwrCtx();
    void run();
    ~AudioDecoder();
private:
    SafeQueue<AVPacket*> *audioQueue;
    AVPacket* packet;
    AVFrame* frame;
    // int out_samples;
    // int len;
    // int out_size;
    AVCodecContext *codeCtx;
    SwrContext *swrCtx;
    uint8_t *pcmOutBuffer;//输出缓冲区
    int outSampleRate; // 输出采样率
    AVSampleFormat outSampleFmt;
    AVChannelLayout  outChLayout;
signals:
    void audioDataReady(QByteArray, double);
};

#endif // AUDIODECODER_H
