#ifndef RTP_H
#define RTP_H

#include <def.h>
#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtppacket.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <dlfcn.h> 
#include "stdint.h"
#include <cassert>
#include <deque>
#include <QtGui>
#include <QThread>
#include "H264/h264.h"
#include "H264/nal_struct.h"

#define RTP_SUPPORT_THREAD
using namespace jrtplib;
using namespace std;
/*******************************************
*mgody
*毕业设计视频传输:
*RTP传输类:将压缩好的数据进行RTP传输
*2015.4.2
*******************************************/


class  RtpSender :public QThread
{
        Q_OBJECT
public:
		RtpSender(void);
   	~RtpSender(void);
		void add_address(const std::string &,uint16_t);
protected:
        void run();     
private:
        RTPSession sess;
        RTPUDPv4TransmissionParams transparams;
				RTPSessionParams sessparams;
        int rtp_iNal ;
				
				struct_nal* struct_pNals;
private: //FU headers struct
        struct FU_INDICATOR
        {//一般x86与DSP、ARM中的CPU都是小端系统，所以相应的在位域中要注意低字节的部分在前，高字节在后。
            unsigned char type:5;
            unsigned char nri:2;
            unsigned char f:1;
        };
        struct FU_HEADER
        {
            unsigned char type:5;
            unsigned char r:1;
            unsigned char e:1;
            unsigned char s:1; 
        };
private:
        bool CheckError(int rtperr);
				string Encode(const unsigned char* Data,int DataByte);
};





#endif
