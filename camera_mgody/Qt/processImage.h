#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include "def.h"
#include <QtGui>
#include <string>
#include <string.h>
#include <iostream>
#include <deque>
#include <iterator>
#include <QThread>
#include <QMutex>
#include "Camera_device/videodevice.h"
#include "H264/h264.h"
#include "Transfer/transfer.h"
using namespace std;
/*******************************************
*mgody
*客户端视频传输:
*主类:负责本地的视频显示以及其他各个类的调用
*2015.3.01//
* 2016.4.02 //去掉所有多余的信号槽。
* 2016.5.25 //更改实现方式。用队列代替信号。
*******************************************/
class H264;
class RtpSender;
#ifndef NDEBUG
class ProcessImage: public QWidget
#else
class ProcessImage: public QObject
#endif
{
   Q_OBJECT
public :
	#ifndef NDEBUG
	ProcessImage(QWidget *parent=0);
	#else
	ProcessImage();
	#endif
   ~ProcessImage();
	static QMutex mutex_yuv422_date_deque ;
	static deque<uchar *>  deque_yuv422_date ;//从头部插入，从尾部输出。
private:
    bool h264_process;
    //bool go_on_signal;
    QLabel *label;
    QImage *frame;
    //QPixmap *frame;
    QTimer *timer;
    int rs;
    uchar *pp;
    uchar * p;
    size_t len;
	VideoDevice *vd;
	H264 *con;
	RtpSender  *rtp;
    int convert_yuv_to_rgb_pixel(int y, int u, int v);
    int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
 	
		int push_frame_date(uchar *);
    //声明其他三个类。


private slots:
	
	#ifndef NDEBUG
    void paintEvent(QPaintEvent *);
    #else
    void Maincirc();
    #endif
   // void display_error(QString err);

};












#endif
