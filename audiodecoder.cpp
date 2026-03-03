#include "audiodecoder.h"

AudioDecoder::AudioDecoder(SafeQueue<AVPacket*> *que,AVCodecContext *ctx):swrCtx(nullptr),pcmOutBuffer(nullptr),outSampleRate(44100),
    outSampleFmt(AV_SAMPLE_FMT_S16),codeCtx(ctx),audioQueue(que){
    // 初始化输出声道布局为立体声（替代旧的 AV_CH_LAYOUT_STEREO）
    av_channel_layout_from_mask(&outChLayout, AV_CH_LAYOUT_STEREO);
}

void AudioDecoder::initSwrCtx()
{
    if (!codeCtx) {
        return;
    }

    int ret = swr_alloc_set_opts2(&swrCtx,
                                 &outChLayout,outSampleFmt,outSampleRate,
                                 &codeCtx->ch_layout,codeCtx->sample_fmt,codeCtx->sample_rate,
                                 0,NULL);
    if(ret<0){
        return;
    }else{
        swr_init(swrCtx);
    }

    pcmOutBuffer = (uint8_t *)av_malloc(44100*2*2);
}

void AudioDecoder::run()
{
    // packet = av_packet_alloc();
    // frame = av_frame_alloc();
    // while(audioQueue->getValue(packet)){
    //     if(avcodec_send_packet(codeCtx,packet) == 0){
    //         while(avcodec_receive_frame(codeCtx,frame) == 0){

    //             out_samples = av_rescale_rnd(swr_get_delay(swrCtx,frame->sample_rate)+frame->nb_samples,
    //                                         outSampleRate,frame->sample_rate,AV_ROUND_UP);

    //             len = swr_convert(swrCtx,
    //                               &pcmOutBuffer,out_samples,
    //                               (const uint8_t**)frame->data,frame->nb_samples);

    //             if (len > 0) {
    //                 // 2. 计算字节大小：采样数 * 通道数(2) * 采样位数(16bit=2字节)
    //                 int out_size = av_samples_get_buffer_size(NULL, 2, len, outSampleFmt, 1);

    //                 // 3. 计算这一帧的时间戳
    //                 double pts = frame->pts * av_q2d(codeCtx->time_base);

    //                 // 4. 发出信号
    //                 emit audioDataReady(QByteArray((const char*)pcmOutBuffer, out_size), pts);
    //             }

    //         }
    //     }
    //     av_packet_unref(packet);
    // }
    // av_packet_free(&packet);
    // av_frame_free(&frame);


    initSwrCtx();

    packet = av_packet_alloc();
    frame = av_frame_alloc();

    // 记录累计播放时间，防止 pts 无效
    double current_pts = 0;

    while(!isInterruptionRequested()){
        if(!audioQueue->getValue(packet)) break;

        if(avcodec_send_packet(codeCtx, packet) == 0){
            while(avcodec_receive_frame(codeCtx, frame) == 0){

                if (frame->pts != AV_NOPTS_VALUE) {
                    current_pts = frame->pts * av_q2d(codeCtx->time_base);
                }
                //计算采样率变化后的输出采样数。
                int out_samples = av_rescale_rnd(swr_get_delay(swrCtx, codeCtx->sample_rate) + frame->nb_samples,
                                                 outSampleRate, codeCtx->sample_rate, AV_ROUND_UP);
                //执行音频转换。转换器上下文,输出缓冲区,输出缓冲区能容纳的采样数,输入数据,输入采样数
                int len = swr_convert(swrCtx, &pcmOutBuffer, out_samples,
                                      (const uint8_t**)frame->data, frame->nb_samples);

                if (len > 0) {
                    int out_size = av_samples_get_buffer_size(NULL, 2, len, outSampleFmt, 1);
                    emit audioDataReady(QByteArray((const char*)pcmOutBuffer, out_size), current_pts);
                    current_pts += (double)len / outSampleRate;
                }
            }
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    av_frame_free(&frame);
}

AudioDecoder::~AudioDecoder()
{
    if (swrCtx) {
        swr_free(&swrCtx);
        swrCtx = nullptr;
    }
    if (pcmOutBuffer) {
        av_freep(&pcmOutBuffer);
        pcmOutBuffer = nullptr;
    }
    // 释放声道布局结构体
    av_channel_layout_uninit(&outChLayout);
}
