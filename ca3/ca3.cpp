// ca3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
extern "C"{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libavutil\pixfmt.h"
#include "libswscale\swscale.h"
}

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) 
{
	char szFilename[32];
	sprintf(szFilename, "frame%d.ppm", iFrame);
	FILE *pFile = fopen(szFilename, "wb");
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);
	for (int y = 0; y<height; y++)
		fwrite(pFrame->data[0] + y*pFrame->linesize[0], 1, width * 3, pFile);
	fclose(pFile);
}
void test()
{
	char *fn = "C:\\logs\\SampleVideo_720x480_2mb.mp4";
	AVFormatContext *pFormatCtx = NULL;
	if (avformat_open_input(&pFormatCtx, fn, NULL, 0) != 0)
		return;
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
		return;
	av_dump_format(pFormatCtx, 0, fn, 0);
	AVStream* video_stream = NULL;
	int video_stream_index = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			video_stream = pFormatCtx->streams[i];
			video_stream_index = i;
		}
	}
	if (video_stream != NULL)
	{
		AVCodec *pCodec = NULL;
		AVCodecContext *pCodecCtx = video_stream->codec;
		pCodec = avcodec_find_decoder(video_stream->codec->codec_id);
		if (pCodec == NULL) 
			return;
		//AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
		if (avcodec_open2(pCodecCtx, pCodec, 0) < 0)
			return;

		AVFrame *pFrame = av_frame_alloc();
		AVFrame *pFrameRGB = av_frame_alloc();
		
		uint8_t *buffer = NULL;
		int numBytes;
		numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
		buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
		avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

		struct SwsContext *sws_ctx = NULL;
		int frameFinished;
		AVPacket packet;
		sws_ctx = sws_getContext(pCodecCtx->width,
			pCodecCtx->height,
			pCodecCtx->pix_fmt,
			pCodecCtx->width,
			pCodecCtx->height,
			AV_PIX_FMT_RGB24,
			SWS_BICUBIC,
			NULL,
			NULL,
			NULL
			);
			
		int i = 0;
		//struct SwsContext *sws_ctx = NULL;
		//int frameFinished;
		//AVPacket packet;
		while (av_read_frame(pFormatCtx, &packet) >= 0)
		{
			if (packet.stream_index == video_stream_index) 
			{
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
				if (frameFinished)
				{
					sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
						pFrame->linesize, 0, pCodecCtx->height,
						pFrameRGB->data, pFrameRGB->linesize);
					if (++i <= 5)
						SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
				}
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	avcodec_register_all();
	test();
	return 0;
}

