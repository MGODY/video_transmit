#ifndef DEF_H
#define DEF_H

//debug  使用ssh远程控制小车的时候不能开启DEBUG模式,也就是必须定义NDEBUG
#define NDEBUG

//Resolution

//#define HD
#ifdef HD
#define pic_width 1280
#define pic_height 720
#define pic_rate 10
#define reflash_time 100
#define constant_bit_rates 1024*5
#else
#define pic_width 640
#define pic_height 480
#define pic_rate 30
#define reflash_time 33.33
#define constant_bit_rates 1024*2
#endif


///rate


//rtp param
#define Destip "192.168.1.104"
#define Destport 1234
#define Baseport 1234
//max packet
#define MAX_RTP_PKT_LENGTH 1350
#define FU_SEND_LENGTH  MAX_RTP_PKT_LENGTH-2
//rtp's type in rtp
#define H264_PT               1 //小车ID
#define time_inc          90000/pic_rate

#endif
