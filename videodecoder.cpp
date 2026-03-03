#include "videodecoder.h"


VideoDecoder::VideoDecoder(SafeQueue<AVPacket*> *vq, AVCodecContext *ctx):
    videoQueue(vq),codeCtx(ctx)
{
    frame = av_frame_alloc();
}

void VideoDecoder::run()
{
    packet = nullptr;
    while(videoQueue->getValue(packet)){
        if(avcodec_send_packet(codeCtx,packet) == 0){
            while(avcodec_receive_frame(codeCtx,frame) == 0){
                // int w = frame->width;
                // int h = frame->height;

                // uchar *y = new uchar[w*h];
                // uchar *u = new uchar[w*h/4];
                // uchar *v = new uchar[w*h/4];
                // for(int i=0; i<h; i++)
                //     memcpy(y + i*w, frame->data[0] + i*frame->linesize[0], w);
                // for(int i=0; i<h/2; i++) {
                //     memcpy(u + i*w/2, frame->data[1] + i*frame->linesize[1], w/2);
                //     memcpy(v + i*w/2, frame->data[2] + i*frame->linesize[2], w/2);
                // }

                sendFrame = av_frame_clone(frame);
                emit frameReady(sendFrame);
                msleep(30);
            }
        }
        // avcodec_send_packet(codeCtx,packet);
        // avcodec_receive_frame(codeCtx,frame);
        av_packet_unref(packet);
    }
}
