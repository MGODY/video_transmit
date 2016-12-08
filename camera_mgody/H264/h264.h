#ifndef H264_H
#define H264_H

#include <def.h>
#include <QtGui>
#include <QThread>
#include <QString>
#include <Qt/processImage.h>
#include <H264/nal_struct.h>
#include <QMutex>
#include <iostream>

using namespace std;

#include <cassert>

#include <string>
#include <stdio.h>

#include <dlfcn.h> 
#include "stdint.h"
#include <vector>
#include <deque>


extern "C"
{
#include "x264.h"
};


/*******************************************
*mgody
*毕业设计视频传输:
*h264编码类:将摄像头获取的视频数据进行编码
*2015.3.20
*******************************************/

class H264 :public QThread 
{
	Q_OBJECT
public:
	H264();
	~H264();
//  bool processing;
	int h264_init();
	static deque<struct_nal*> x264_nal_deque ;//从头部插入，从尾部输出。
	static deque<int> x264_inal_deque ;
	static QMutex mutex_x264_nal_deque ;
	//static int static_iNal;
protected:
	void run();

private:
	int iNal ;
	x264_nal_t* pNals ;
	unsigned int pic_pix_len;
	x264_t* pX264Handle;
	x264_param_t* pX264Param;
	x264_picture_t* pPicIn;
	x264_picture_t* pPicOut;
	int iResult; 
	unsigned char * yuv420;
	unsigned char * yuv422;
	int sps_pps_i;
	x264_nal_t* sps_pps_nals;
	unsigned int g_uiPTSFactor;
	int convert_yuv422_to_yuv420(unsigned char *,unsigned char *,unsigned int ,unsigned int);
};






#endif
