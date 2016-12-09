#ifndef DECODE_H
#define DECODE_H

extern "C"
{
//#include <libavutil/opt.h>
#include "libavcodec/avcodec.h"
//#include <libavutil/channel_layout.h>
//#include <libavutil/common.h>
//#include <libavutil/imgutils.h>
//#include <libavutil/mathematics.h>
//#include <libavutil/samplefmt.h>
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
};
#include "define.h"
#include "Accept/accept.h"
#include <QThread>
#include <QtGui>
#include <QMutex>
#include <string>
#include <iostream>
#include <assert.h>
#include <math.h>
using namespace std;

struct Pic_Date
{
	Pic_Date(int w,int h)
	{
		p_date = new unsigned char[w*h*3];
		width = w;
		height = h;
		is_size_changed = false;
		i_pt = 0;
	}
	Pic_Date()
	{
		p_date = new unsigned char[1080*720*3];
		width = 1080;
		height = 720;
		is_size_changed = false;
	}
	
	~Pic_Date()
	{
		if(p_date!=NULL)
		{
			delete []p_date;
			p_date = NULL;
		}
	}
	unsigned char * p_date;
	int width;
	int height;
	long i_pt;
	bool is_size_changed;
};

struct Frame_Deque
{
	//有时候必须在占用内存与占用CPU之间做一个折中。
	Frame_Deque()
	{//频繁的申请内存并不好，所以对于队列我们干脆直接申请足够使用的空间。
	//使用默认构造函数。
		p_frame = new Pic_Date[10];
		data_i = 0;
		data_p = 0;
		data_f = 0;
	}
	~Frame_Deque()
	{
		if(p_frame!=NULL)
		{
			delete []p_frame;
		}
	}
	QMutex mutex;
	Pic_Date* p_frame;
	int push_frame(AVFrame*,int);
	Pic_Date* get_frame(Pic_Date*);
	bool isnull();
private:
	unsigned int data_i ;//Pic_Date 的个数
	int data_p ;//Pic_Date 的位置
	unsigned int data_f ;
};


class Decode_video :public QThread
{
	Q_OBJECT
public:
	Decode_video(int );
	~Decode_video();
	Frame_Deque* p_frame_deque;
protected:
	void run();
private :
	int decodehandle;
	AVCodec *codec;
	AVCodecContext *c= NULL;
	int frame,got_picture, len;
	AVFrame* picture;
	AVFrame* RGB_pic;
	AVPacket avpkt;
	CVideoData * p_nall;
	static SwsContext *scxt;//将scxt声明在函数外边，不然会引起内存泄露
	void Call_Show();
};


#endif
