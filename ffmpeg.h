#ifndef __FFMPEG___
#define __FFMPEG___ 1
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

int outputJpeg(const char * out_file , AVPacket * outpkt, int width, int height, AVFrame* frame, AVPacket * pkt);
int process(float tm, const char * url, const char* tofile);
void init();

#endif

