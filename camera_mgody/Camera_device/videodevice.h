#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H
#include <def.h>
#include<iostream>
using std::cout;
using std::endl;

#include<fcntl.h>
#include<errno.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>


#include<sys/ioctl.h>
#include<sys/mman.h>


#include<asm/types.h>
#include<linux/videodev2.h>

#include <QString>
#include <QObject>


#define CLEAR(x) memset(&(x), 0, sizeof(x))
/*******************************************
*mgody
*毕业设计视频传输:
*摄像头类:从摄像头获取视频流
*2015.3.20
*******************************************/
class VideoDevice: public QObject
{
   Q_OBJECT
public: 
   VideoDevice(QString device_name);
   int open_device();
   int init_device();
   int start_capture();
   int get_frame(void **, size_t *);
   int go_on_frame();
   int stop_capture();
   int unmap();
   int close_device();
private:
   int init_mmap();
   int fd;
   QString dev_name;
   struct buffer
    {
        void * start;
        size_t length;
    };
    buffer* buffers;
    unsigned int n_buffers;
    int index;
signals:
   //void display_error(QString);
   //void display(QString a);
   
};

#endif
