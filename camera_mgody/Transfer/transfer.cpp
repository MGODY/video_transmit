#include "Transfer/transfer.h"

RtpSender::RtpSender(void)
{       
        
        int status;
        rtp_iNal = 0;
        //flags_420=1;
        struct_pNals = NULL;
     	sessparams.SetOwnTimestampUnit(1.0/90000);		
		sessparams.SetAcceptOwnPackets(true);
		transparams.SetPortbase(Baseport);

		status = sess.Create(sessparams,&transparams);	
		CheckError(status);
        cout<<"rtp init ok"<<endl;
  //sess.SetDefaultPayloadType(H264_PT);
	//sess.SetDefaultMark(true);
	//sess.SetDefaultTimestampIncrement(90000/pic_rate);

}

RtpSender::~RtpSender(void)
{
	struct_pNals = NULL;
}
bool RtpSender::CheckError(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout<<"ERROR: "<<RTPGetErrorString(rtperr)<<std::endl;
		return false;
	}
	return true;
}

void RtpSender::run()
{
	while(1)
	{
	
	//cout<<"in rtp"<<endl;
	unsigned char * sendbuff;
	unsigned int len;
	//unsigned int time_inc=90000/picrate;
	FU_INDICATOR *fu_ind;
	FU_HEADER    *fu_hdr;
	int status;
	H264::mutex_x264_nal_deque.lock();
	if(!H264::x264_nal_deque.empty())
	{//RTP打包发送
		//cout<<"rtp begin   "<<endl;
		rtp_iNal = H264::x264_inal_deque.back();
		H264::x264_inal_deque.pop_back();
		struct_pNals = (struct_nal*)malloc(rtp_iNal*sizeof(struct_nal));
		for(int i=1;i<=rtp_iNal;i++)
		{
			//cout<<"rtp sender test "<<i<<endl;
			struct_pNals[i-1].i_payload = H264::x264_nal_deque.back()[i-1].i_payload;
			//cout<<"payload date is "<<struct_pNals[i-1].i_payload <<endl;
			struct_pNals[i-1].p_payload = (unsigned char*)malloc(struct_pNals[i-1].i_payload*sizeof(char));
			memcpy(struct_pNals[i-1].p_payload,H264::x264_nal_deque.back()[i-1].p_payload,struct_pNals[i-1].i_payload);
			struct_pNals[i-1].i_type = H264::x264_nal_deque.back()[i-1].i_type;
			
		}
		for(int i=1;i<=rtp_iNal;i++)
		{//释放内存.
			free(H264::x264_nal_deque.back()[i-1].p_payload);
			H264::x264_nal_deque.back()[i-1].p_payload = NULL;
		}
		free(H264::x264_nal_deque.back());
		H264::x264_nal_deque.back()=NULL;
		H264::x264_nal_deque.pop_back();
		H264::mutex_x264_nal_deque.unlock();

        for( int i=0;i<rtp_iNal;++i){
			
             //去除前导码 0x00 00 01   0x00 00 00  01
                if(0x01==struct_pNals[i].p_payload[2]){
                        sendbuff=&struct_pNals[i].p_payload[3];
 
                        len=struct_pNals[i].i_payload-3;
                }
                else{
                        sendbuff=&struct_pNals[i].p_payload[4];
                        len=struct_pNals[i].i_payload-4;
                }
                char sendbuf[1500];   //发送的数据缓冲
	        	memset(sendbuf,0,1500);

 
                if ( len <= MAX_RTP_PKT_LENGTH ){  
		        memcpy(sendbuf,sendbuff,len);  
		        status =sess.SendPacket((void *)sendbuf,len,H264_PT,true,time_inc);
		        CheckError(status);

	        }  
	        else if(len > MAX_RTP_PKT_LENGTH){//if the packet is largger than the MTU divide it into FUs;
                        --len;
                        unsigned int k ,last;
                        k=len/(MAX_RTP_PKT_LENGTH-2);
                        last=len%(MAX_RTP_PKT_LENGTH-2);
                        unsigned int i_send=0;
                        while(i_send<k){
                           
                               memcpy(sendbuf+2,sendbuff+i_send*(MAX_RTP_PKT_LENGTH-2)+1,(MAX_RTP_PKT_LENGTH-2));
                               //set fu indicator
                               fu_ind=(FU_INDICATOR*)&sendbuf[0];
                               fu_ind->f=0;
                               fu_ind->nri=sendbuff[0]>>5;
                               fu_ind->type=28;
                               //set fu header
                               fu_hdr=(FU_HEADER*)&sendbuf[1];
                               if(0==i_send)
                                      fu_hdr->s=1;
                               else   
                                      fu_hdr->s=0;
                               fu_hdr->e=0;
                               fu_hdr->r=0;
                               fu_hdr->type=struct_pNals[i].i_type;
                               //send packege
                               if(0==i_send){
                                       status =sess.SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH,H264_PT,false,time_inc);
		                       CheckError(status);
                               }else{
                                       status =sess.SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH,H264_PT,false,0);
		                       CheckError(status);
                               }
                               ++i_send;
                        }
			if(last){
		                memcpy(sendbuf+2,sendbuff+i_send*(MAX_RTP_PKT_LENGTH-2)+1,last);
		                fu_ind=(FU_INDICATOR*)&sendbuf[0];
		                fu_ind->f=0;
		                fu_ind->nri=sendbuff[0]>>5;
		                fu_ind->type=28;
		                //set fu header
		                fu_hdr=(FU_HEADER*)&sendbuf[1];
		                fu_hdr->s=0;
		                fu_hdr->e=1;
		                fu_hdr->r=0;
		                fu_hdr->type=struct_pNals[i].i_type;
		                status =sess.SendPacket((void *)sendbuf,last+2,H264_PT,true,0);
				CheckError(status);
			}
                }
        }  //for(unsigned int i=0;i<iNal;++i){
        //cout<<"rtp send free nals"<<endl;	
		for(int i=rtp_iNal;i<=rtp_iNal;i++)
		{//释放内存.
			free(struct_pNals[i-1].p_payload);
			struct_pNals[i-1].p_payload = NULL;
		}
		free(struct_pNals);
		struct_pNals=NULL;
		#ifndef NDEBUG
		cout<<"rtp send succesefully"<<endl;
		#endif
	}else
	{		
			#ifndef NDEBUG
			cout<<"x264_nal_deque is empty,wait 30ms"<<endl;
			#endif
			H264::mutex_x264_nal_deque.unlock();
			msleep(30);
	}
	
	//exec();
	}//while(1);
}

void RtpSender::add_address(const std::string &destipaddr,uint16_t destport)
{       
        int status;
        uint32_t destip;
        destip = inet_addr(destipaddr.c_str());
        destip = ntohl(destip);
        RTPIPv4Address addr(destip,destport);
        status = sess.AddDestination(addr);
        CheckError(status);
        cout<<"get address"<<endl;
}

               



string RtpSender::Encode(const unsigned char* Data,int DataByte)
{
    //编码表
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //返回值
    string strEncode;
    unsigned char Tmp[4]={0};
    int LineLength=0;
    for(int i=0;i<(int)(DataByte / 3);i++)
    {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        Tmp[3] = *Data++;
        strEncode+= EncodeTable[Tmp[1] >> 2];
        strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
        strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
        strEncode+= EncodeTable[Tmp[3] & 0x3F];
        if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}
    }
    //对剩余数据进行编码
    int Mod=DataByte % 3;
    if(Mod==1)
    {
        Tmp[1] = *Data++;
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
        strEncode+= "==";
    }
    else if(Mod==2)
    {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
        strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
        strEncode+= "=";
    }
    
    return strEncode;
}























