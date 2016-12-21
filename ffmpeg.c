#include "ffmpeg.h"
#include <stdbool.h>

static bool is_init = false;

void init()
{
	if (!is_init)
	{
		av_register_all();
		avcodec_register_all();
		is_init = true;
	}
}

int process(float tm, const char * url, const char* tofile)
{
	init();

	AVFormatContext *ps = avformat_alloc_context();
	if (avformat_open_input(&ps, url, NULL, NULL) != 0) 
	{
		avformat_free_context(ps);
		return -1;
	}
	if (avformat_find_stream_info(ps, NULL) < 0)
	{
		avformat_close_input(&ps);
		avformat_free_context(ps);
		return -2;
	}
	float len = (float)((ps->duration + 5000) / AV_TIME_BASE);
	if ( tm > len )
	{
		avformat_close_input(&ps);
		avformat_free_context(ps);
		return -3;
	}
	int i;
	int videoStream = -1;
	for(i=0; i<ps->nb_streams; i++)
	{
		if(ps->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO )
		{
			videoStream = i;
			break;
		}
	}
	if (videoStream == -1)
	{
		avformat_close_input(&ps);
		avformat_free_context(ps);
		return -4;
	}
	AVCodec *vCodec = avcodec_find_decoder(ps->streams[videoStream]->codecpar->codec_id);
	AVCodecContext *avcCtx = avcodec_alloc_context3(vCodec);
	avcodec_parameters_to_context(avcCtx, ps->streams[videoStream]->codecpar);
	avcodec_open2(avcCtx, vCodec, NULL);
	int64_t f = av_rescale(tm, ps->streams[videoStream]->time_base.den, ps->streams[videoStream]->time_base.num);
	int b = avformat_seek_file(ps, videoStream, 0, f, f, AVSEEK_FLAG_FRAME);
	AVFrame* frame = av_frame_alloc();
	avcodec_flush_buffers(avcCtx);
	AVPacket *pkt = av_packet_alloc();
	while (av_read_frame(ps, pkt) == 0)
	{
		if (pkt->stream_index != videoStream) {
			av_packet_unref(pkt);
			continue;
		}
		avcodec_send_packet(avcCtx, pkt);
		if (pkt->dts < f)
		{
			av_packet_unref(pkt);
			continue;
		}
		int ret = avcodec_receive_frame(avcCtx, frame);
		if (ret != 0 )
		{
			av_packet_unref(pkt);
			continue;
		}
		int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, avcCtx->width, avcCtx->height, 1);
		AVPacket outpkt;
		av_new_packet(&outpkt,numBytes);
		outputJpeg(tofile,  &outpkt, avcCtx->width, avcCtx->height, frame, pkt);
		av_packet_unref(pkt);
		break;
	}
	av_packet_free(&pkt);
	av_frame_free(&frame);
	avcodec_close(avcCtx);
	avcodec_free_context(&avcCtx);
	avformat_close_input(&ps);
	avformat_free_context(ps);
	return 0;
}


int outputJpeg(const char * out_file , AVPacket * outpkt, int width, int height, AVFrame* frame, AVPacket * pkt)
{

	AVFormatContext *out = avformat_alloc_context();
	AVCodecContext* outCodecCtx;
	AVCodec* outCodec;
	AVOutputFormat* fmt = av_guess_format("mjpeg", ".jpg", "image/jpeg");
	AVStream* video_st;
	out->oformat = fmt;
	avio_open(&out->pb , out_file, AVIO_FLAG_READ_WRITE);
	video_st = avformat_new_stream(out, NULL);
	outCodec = avcodec_find_encoder(out->oformat->video_codec);
	outCodecCtx = avcodec_alloc_context3(outCodec);
	outCodecCtx->codec_id = out->oformat->video_codec;
	outCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	outCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
	outCodecCtx->width = width;
	outCodecCtx->height = height;
	outCodecCtx->time_base.num = 1;
	outCodecCtx->time_base.den = 25;
	outCodecCtx->global_quality = 100;
	avcodec_parameters_from_context(video_st->codecpar, outCodecCtx);
	avcodec_open2(outCodecCtx, outCodec, NULL);
	int r = avformat_write_header(out,NULL);
	avcodec_send_frame(outCodecCtx, frame);
	int ret = avcodec_receive_packet(outCodecCtx, outpkt);
	outpkt->pts = pkt->pts;
	outpkt->stream_index = 0;
	av_write_frame(out, outpkt);
	av_write_trailer(out);
	av_packet_unref(outpkt);
	avcodec_close(outCodecCtx);	
	avcodec_free_context(&outCodecCtx);
	avformat_close_input(&out);
	avformat_free_context(out);
	return 0;
}

