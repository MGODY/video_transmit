#ifndef ACCEPT_H
#define ACCEPT_H

//#include "jthread.h"
//#include "jmutexautolock.h"
//#include "jmutex.h"
#include "define.h"
#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtppacket.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#ifndef WIN32
	#include <netinet/in.h>
	#include <arpa/inet.h>
#else
	#include <winsock2.h>
#endif // WIN32
#include <jrtplib3/rtpsourcedata.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>


#include <QThread>
#include <QtGui>
#include <QMutex>
using namespace jrtplib;
using namespace std;
//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

void checkerror(int rtperr);

//
// The new class routine
//

class CVideoData{
public:
	unsigned char * m_pBuffer;
	long m_lLength;
	long m_lMaxLength;
	uint64_t m_pts;
	CVideoData(){m_pBuffer = new unsigned char[BUFFER_SIZE];m_lLength = 0; m_pts = 0;m_lMaxLength = BUFFER_SIZE;}
	CVideoData(long buffer){m_lLength = buffer;m_pts = 0;m_pBuffer = new unsigned char[buffer];m_lMaxLength = buffer;}
	~CVideoData(){
		if(m_pBuffer != NULL)
		{
			delete []m_pBuffer;
			m_pBuffer = NULL;
		}
		m_lLength = 0;
	}
};
class CThreadSafeArray
{
public :
	void add(CVideoData *);
	CVideoData* get(CVideoData*);
	CThreadSafeArray();
	~CThreadSafeArray();
	bool isnull();
	QMutex mutex;
	CVideoData *datapull;
	unsigned int data_i;//CVideoData 的个数
	int data_p ;//CVideoData 的位置
	unsigned int data_f;
};
struct SPS_PPS
{	
	SPS_PPS()
	{
		p_data = new unsigned char[1000];
		length = 0;
	}
	~SPS_PPS()
	{
		delete []p_data;
	}
	unsigned char* p_data;
	int length;
};
struct FU_INDICATOR
{//1、fu_indicator ,这里需要注意的是顺序是从低位到高位的，所以与我们看到的实际中的FU_INDICATOR顺序看起来是相反的。
//2、由于采用了分包发送，并且加入了FU_INDICATOR与FU_HEADER，在收到包以后必须重新组成NALU单元来解码，并且还需要加入包头0x00000001
//3、所以代码中需要做的是1、加入包头 2、还原nalu头部 3、去掉原来的FU_INDICATOR与FU_HEADER 。
//4、之所以分包发送的时候去掉头部并且加上FU_INDICATOR与FU_HEADER是为了与VLC播放器兼容，这样视频流既可以直接使用播放器播放，也可以使用
//客户端来播放。
//5、nalu头部的结构是和FU_INDICATOR的结构相同的。 只不过type的值不同而已。
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

class MyRTPSession : public QThread// ,public QObject
{
	Q_OBJECT
public :
	static CThreadSafeArray *m_ReceiveArray;
	MyRTPSession(void);
	~MyRTPSession(void);
protected:
	void run();
private:

	//void OnPollThreadStep();
	void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
	RTPSession sess;
	//init_rtpsess(sess);
	SPS_PPS *p_sps_pps;
	CVideoData* m_pVideoData;
	unsigned char m_buffer[BUFFER_SIZE];
	int m_current_size;
	int status;

};








#endif
