#include "videothread.h"



VideoThread::VideoThread() {}

void VideoThread::run()
{
    std::string fileName = "H:\\download\\ffmpeg-mux-data\\juren.mp4";
    AVFormatContext *avContext = nullptr;
    avformat_open_input(&avContext,fileName.c_str(),nullptr,nullptr);
    avformat_find_stream_info(avContext,nullptr);
    int videoIndex = -1;
    for(int i=0 ; i<avContext->nb_streams ; i++){
        if(avContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoIndex = i;
            break;
        }
    }
    if(videoIndex == -1){
        return;
    }
    AVCodecParameters* codePar = avContext->streams[videoIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codePar->codec_id);
    AVCodecContext *codeContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codeContext,codePar);
    avcodec_open2(codeContext,codec,nullptr);

    AVFrame *frame = av_frame_alloc();
    AVPacket *pkt = av_packet_alloc();

    while(!isInterruptionRequested()){
        if(av_read_frame(avContext,pkt)<0) break;
        if(pkt->stream_index == videoIndex){
            if(avcodec_send_packet(codeContext,pkt) == 0){
                while(avcodec_receive_frame(codeContext,frame) == 0){
                    int w = frame->width;
                    int h = frame->height;

                    uchar* y = new uchar[w * h];
                    uchar* u = new uchar[w * h / 4];
                    uchar* v = new uchar[w * h / 4];

                    // 注意：必须逐行拷贝，以处理 linesize != width 的情况
                    for(int i=0; i<h; i++)
                        memcpy(y + i*w, frame->data[0] + i*frame->linesize[0], w);
                    for(int i=0; i<h/2; i++) {
                        memcpy(u + i*w/2, frame->data[1] + i*frame->linesize[1], w/2);
                        memcpy(v + i*w/2, frame->data[2] + i*frame->linesize[2], w/2);
                    }

                    emit frameReady(y, u, v, w, h);

                    // 控制一下速度，暂时先硬编码一个延迟（后面要改用 PTS 同步）
                    msleep(30);
                }
            }
        }
        av_packet_unref(pkt);
    }
}
