#include "demuxthread.h"


DemuxThread::DemuxThread(SafeQueue<AVPacket *> *vq, std::string name):
    videoStream(-1),running(true),videoQueue(vq),fileName(name)
{
    formatCtx = avformat_alloc_context();
    avformat_open_input(&formatCtx,fileName.c_str(),nullptr,nullptr);
    for(int i=0;i<formatCtx->nb_streams;i++){
        if(formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStream = i;
            break;
        }
    }

    codePara = formatCtx->streams[videoStream]->codecpar;
    codec = avcodec_find_decoder(codePara->codec_id);
    codeCtx = avcodec_alloc_context3(codec);
    // 将编解码参数复制到解码器上下文中
    avcodec_parameters_to_context(codeCtx,codePara);
    // 打开编解码器
    avcodec_open2(codeCtx,codec,nullptr);
}

void DemuxThread::run()
{
    while(!isInterruptionRequested()){
        packet = av_packet_alloc();
        if (av_read_frame(formatCtx, packet) == 0) {
            if (packet->stream_index == videoStream) {
                videoQueue->putValue(packet); // 塞进公共队列
            } else {
                av_packet_free(&packet);
            }
        }else{
            av_packet_free(&packet);
            break;
        }
    }
}

AVCodecContext *DemuxThread::getCodeCtx()
{
    return codeCtx;
}

