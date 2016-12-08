
#include"Accept/accept.h"
CThreadSafeArray *MyRTPSession::m_ReceiveArray = new CThreadSafeArray;
void CThreadSafeArray::add(CVideoData *a)
{
	mutex.lock();
	memcpy(datapull[data_p].m_pBuffer,a->m_pBuffer,a->m_lLength);
	datapull[data_p].m_lLength = a->m_lLength;
	
	memset(a->m_pBuffer, 0, datapull[data_p].m_lLength); //将入口的CVideoData清空缓存
	a->m_lLength = 0;
	data_p=(data_p+1)%10;
	
	if(10==data_i)
	{//队列已经满了
		cout<<"the CVideoData array is full"<<endl;
		data_f  = (data_f + 1) % 10;
	}else
	{
		data_i++;
	}
	mutex.unlock();
}
CVideoData* CThreadSafeArray::get(CVideoData*a)
{
	mutex.lock();
	CVideoData * b=datapull+data_f;
	memcpy(a->m_pBuffer,b->m_pBuffer,b->m_lLength);
	a->m_lLength = b->m_lLength;
	data_f  = (data_f + 1) % 10;
	data_i--;
	mutex.unlock();
	return b;
}
bool CThreadSafeArray::isnull()
{
	mutex.lock();
	if(0 == data_i)
	{
		mutex.unlock();
		return true;
	}else
	{
		mutex.unlock();
		return false;
	}	
	return true ;
}
CThreadSafeArray::CThreadSafeArray()
{
	//就分配内存来说，下面这句体现了C++中用new比malloc的优势的地方，调用new可以使用对象的构造函数，所以构造函数中的
	//分配内存的代码部分都会被执行，而使用malloc则不行，由于没有执行构造函数，m_pBuffer的指针是没有被分配内存的，所以还得
	//继续给m_pBuffer指针分配内存。
	//malloc分配内存的优势应该是在大容量分配内存的时候，由于不会给分配初值，所以分配速度会比new快，而C++也有相应的malloc模板，来
	//达到相同的功能。这里使用的是固定循环队列，所以暂不去管。
	datapull = new CVideoData[10];
	data_i = 0;
	data_p = 0;
	data_f = 0;
}
CThreadSafeArray::~CThreadSafeArray()
{
	if (datapull != NULL)
	{
		delete []datapull;
		datapull = NULL;
		
	}
}


MyRTPSession::MyRTPSession(){
//	m_ReceiveArray = new CThreadSafeArray();
	m_pVideoData = new CVideoData();
	p_sps_pps = new SPS_PPS;
	int status;
	memset(m_buffer,0,BUFFER_SIZE);
	m_current_size = 0;
	//frist_pack_flag = 0;
	uint16_t portbase;
	std::string ipstr;

	portbase = PORT_BASE;
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	sessparams.SetOwnTimestampUnit(1.0/90000.0);
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);
	
	status = sess.Create(sessparams,&transparams);
	checkerror(status);
	cout<<"rtp init "<<endl;

}
MyRTPSession::~MyRTPSession()
{
	delete p_sps_pps;
	p_sps_pps = NULL;
	delete m_pVideoData;
	m_pVideoData = NULL;
}


void MyRTPSession::run()
{
	while(1){
		sess.BeginDataAccess();
		
		// check incoming packets
		if (sess.GotoFirstSourceWithData())
		{
			cout<<"rtp accept test!!!"<<endl;
			//frist_pack_flag = 1;
			do
			{
				RTPPacket *pack;
				RTPSourceData *srcdat;
			
				srcdat =sess.GetCurrentSourceInfo();
			
				while ((pack = sess.GetNextPacket()) != NULL)
				{
					ProcessRTPPacket(*srcdat,*pack);
					sess.DeletePacket(pack);
				}
				} while (sess.GotoNextSourceWithData());
		}
		
		sess.EndDataAccess();
		status = sess.Poll();
		checkerror(status);
	}
}
void MyRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	// You can inspect the packet and the source's info here
	if(rtppack.GetPayloadType() == H264_PT){
		
		if(rtppack.HasMarker()){//如果是最后一包则进行组包
			//m_pVideoData->m_lLength = m_current_size + rtppack.GetPayloadLength();//得到数据包总的长度
			//这里曾犯的一个错误是，对于SPS与PPS包或者其他由于本身是没有进行分包的所以这里处理的时候首先要判断包的类型。
			
			//添加前导码
			m_pVideoData->m_pBuffer[0] = 0x00;
			m_pVideoData->m_pBuffer[1] = 0x00;
			m_pVideoData->m_pBuffer[2] = 0x00;
			m_pVideoData->m_pBuffer[3] = 0x01;
			
			FU_INDICATOR* nal_hdr;
			nal_hdr = (FU_INDICATOR*)&m_pVideoData->m_pBuffer[4];
			nal_hdr->f = 0;
			nal_hdr->nri = rtppack.GetPayloadData()[0]>>5;
			nal_hdr->type = rtppack.GetPayloadData()[0];
			if(nal_hdr->type == 28)
			{//这是一个分包
				cout<<"收到分包"<<endl;
				nal_hdr->type = rtppack.GetPayloadData()[1];
				memcpy(m_buffer + m_current_size,rtppack.GetPayloadData()+2,rtppack.GetPayloadLength()-2);
				m_current_size = rtppack.GetPayloadLength() + m_current_size - 2;
				
				memcpy(m_pVideoData->m_pBuffer + 5,m_buffer,m_current_size);
				m_pVideoData->m_lLength = m_current_size + 5;
			}else{//没有进行分包的。
				//cout<<"收到SPS或者PPS"<<endl;
				if(nal_hdr->type == 7)
				{
					cout<<"get sps"<<endl;
				}
				if(nal_hdr->type == 8)
				{
					cout<<"get pps"<<endl;
				}
				memcpy(m_buffer + m_current_size,rtppack.GetPayloadData()+1,rtppack.GetPayloadLength()-1);
				m_current_size = rtppack.GetPayloadLength() + m_current_size - 1;
				
				memcpy(m_pVideoData->m_pBuffer + 5,m_buffer,m_current_size);
				m_pVideoData->m_lLength = m_current_size + 5;
				
			}
			
			m_ReceiveArray->add(m_pVideoData);//添加到接收队列
			std::cout<<"get pack"<<std::endl;
			memset(m_buffer,0,m_current_size+5);//清空缓存，为下次做准备
			m_current_size = 0;
		}else{
				//放弃前两个字节，因为这两个字节是分包头的两个字节，要组成完整的nalu，这两个直接丢弃即可。
				memcpy(m_buffer + m_current_size,rtppack.GetPayloadData()+2,rtppack.GetPayloadLength()-2);
				m_current_size = m_current_size + rtppack.GetPayloadLength() - 2;
			
		}
	} 
	
}




void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}


