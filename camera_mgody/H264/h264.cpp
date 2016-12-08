#include"H264/h264.h"

deque<struct_nal* >  H264::x264_nal_deque ;
QMutex H264::mutex_x264_nal_deque ;
deque<int> H264::x264_inal_deque ;
H264::H264()
{
  pX264Handle   = NULL;
  pX264Param = new x264_param_t;
  assert(pX264Param);
  iNal = 0;
  pNals = NULL;
  pPicIn = new x264_picture_t;
  pPicIn->i_pts=0;
  pPicOut = new x264_picture_t;
  iResult = 0;
  pic_pix_len = pic_width*pic_height;
  yuv420 = (unsigned char *)malloc(pic_width * pic_height* 3/2 * sizeof(char));
  yuv422 = (unsigned char *)malloc(pic_width * pic_height/*QWidget::width()*QWidget::height()*/* 2 * sizeof(char));
}
int H264::h264_init()
{
   //* 使用默认参数
 //"ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow", "placebo"
//"film", "animation", "grain", "stillimage", "psnr", "ssim", "fastdecode", "zerolatency",
 x264_param_default_preset(pX264Param, "fast", "zerolatency");
 //* cpuFlags
 pX264Param->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;//* 取空缓冲区继续使用不死锁的保证.
 //* video Properties 较为重要的编码基本设置
 pX264Param->i_width   = pic_width; //* 宽度.
 pX264Param->i_height  = pic_height; //* 高度
 pX264Param->i_frame_total = 0; //* 编码总帧数.不知道用0.
 pX264Param->i_keyint_max = pic_rate*7;//关键帧的帧间隔
 pX264Param->i_keyint_min = pic_rate;
 
 //* bitstream parameters 对于基本档次的编码是没有b帧的，只有I和P帧，这里理论上是不需要设置的。
 //为了设置的完整性，做出一些设置。
 //pX264Param->i_bframe  = 3;
 pX264Param->b_open_gop  = 0;
 pX264Param->i_bframe_pyramid = 2;
 pX264Param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
 pX264Param->b_repeat_headers = 1;
 //pX264Param->b_cabac = 1;
 


 //* Log选择是否要开启debug
//pX264Param->i_log_level  = X264_LOG_DEBUG;

/**** Rate control Parameters
*码率控制，对于实时传输来说非常重要的设置。
*对于实时视频传输来说一般设置为bitrate模式，来直接控制码率
*可用的选择有ABR（平均码率控制）、CRF(固定质量)、CQP（固定量化）
***************/
 //pX264Param->rc.i_lookahead = 0;
 pX264Param->rc.b_mb_tree = 0;//保证延迟
 
 //pX264Param->rc.f_rf_constant = 25;//在CQP下调整这两个值，会影响数据量。
 //pX264Param->rc.f_rf_constant_max = 45;
 pX264Param->rc.i_rc_method = X264_RC_ABR;
 
 pX264Param->rc.i_bitrate = constant_bit_rates;//* 码率(比特率,单位Kbps)
 pX264Param->rc.i_vbv_max_bitrate = constant_bit_rates*1.2;
 pX264Param->rc.i_vbv_buffer_size = constant_bit_rates*1.2;
 //* muxing parameters
 pX264Param->i_fps_den  = 1; //* 帧率分母
 pX264Param->i_fps_num  = (int)pic_rate;//* 帧率分子
 pX264Param->i_timebase_den = pX264Param->i_fps_num;
 pX264Param->i_timebase_num = pX264Param->i_fps_den;

 //* 设置Profile.编码标准，可以设置为基本、主要、扩展三个档次，对于实时编码这里选择baseline也就是基本档次
 x264_param_apply_profile(pX264Param,"baseline");

 //* 打开编码器句柄,通过x264_encoder_parameters得到设置给X264
 //* 的参数.通过x264_encoder_reconfig更新X264的参数
 pX264Handle = x264_encoder_open(pX264Param);
 assert(pX264Handle);
 //x264_encoder_parameters(pX264Handle,pX264Param);

 //* 获取整个流的PPS和SPS,不需要可以不调用.
 iResult = x264_encoder_headers(pX264Handle, &sps_pps_nals, &sps_pps_i);
 //传递sps pps　参数
 cout<<"sps_pps_i : "<<sps_pps_i<<endl;
 //emit trans_sps_pps(sps_pps_nals,sps_pps_i);
 assert(iResult >= 0);

 //* 获取允许缓存的最大帧数.
// int iMaxFrames = x264_encoder_maximum_delayed_frames(pX264Handle);

 //* 编码需要的参数.
 int iMaxFrames = x264_encoder_maximum_delayed_frames(pX264Handle);
 cout<<"iMaxFrames"<<iMaxFrames<<endl;
 x264_picture_init(pPicOut);
 x264_picture_alloc(pPicIn, X264_CSP_I420, pX264Param->i_width, pX264Param->i_height);
 pPicIn->img.i_csp = X264_CSP_I420;
 pPicIn->img.i_plane = 3;

 return 0;
}
H264::~H264()
{
   //* 清除图像区域
 x264_picture_clean(pPicIn);
 x264_picture_clean(pPicOut);
 //* 关闭编码器句柄
 x264_encoder_close(pX264Handle);
 pX264Handle = NULL;

 delete pPicIn ;
 pPicIn = NULL;

 delete pPicOut;
 pPicOut = NULL;

 delete pX264Param;
 pX264Param = NULL;
 
 free(yuv420);
 yuv420 = NULL;
 
 free(yuv422);
 yuv422 = NULL;

}
 void H264::run()
{
 
  while(1){
  	 int iresult = 0;
 
    //cout<<"in h264"<<endl;
	ProcessImage::mutex_yuv422_date_deque.lock();
	if(!ProcessImage::deque_yuv422_date.empty())
	{
		//cout<<"h264 p"<<endl;
		memcpy(yuv422,ProcessImage::deque_yuv422_date.back(),pic_width * pic_height* 2);
		free(ProcessImage::deque_yuv422_date.back());
		ProcessImage::deque_yuv422_date.back()=NULL;
		ProcessImage::deque_yuv422_date.pop_back();
		ProcessImage::mutex_yuv422_date_deque.unlock();
		
		convert_yuv422_to_yuv420(yuv422,yuv420,pic_width,pic_height);
		
		memcpy(pPicIn->img.plane[0], yuv420, pic_pix_len);
		memcpy(pPicIn->img.plane[1], yuv420+pic_pix_len, pic_pix_len/4 );
		memcpy(pPicIn->img.plane[2], yuv420+pic_pix_len+pic_pix_len/4, pic_pix_len/4);
		//cout<<"h264 p0"<<endl;
		
		iresult=x264_encoder_encode(pX264Handle, &pNals, &iNal, pPicIn, pPicOut);
		pPicIn->i_pts++;
		#ifndef NDEBUG
		if (iresult==0)
		{
			cout<<"iresult is zero"<<endl;
		}
		cout<<"H264 transfer successfully get "<<iNal<<"nals"<<endl;
		#endif

		
		struct_nal* nal_date;
		nal_date = (struct_nal*)malloc(iNal*sizeof(struct_nal)); 
		
		#ifndef NDEBUG
		if (nal_date==NULL)
		{
			cout<<"nal_date ptr is NULL!!!"<<endl;
		}
		#endif
		for(int i=1;i<=iNal;i++)
		{
			nal_date[i-1].p_payload = (unsigned char *)malloc(pNals[i-1].i_payload*sizeof( char));
			#ifndef NDEBUG
			if (nal_date[i-1].p_payload==NULL)
			{
				cout<<"nal_date"<<"["<<i-1<<"].p_payload is NULL!!!"<<endl;
			}
			#endif
			memcpy(nal_date[i-1].p_payload,pNals[i-1].p_payload,pNals[i-1].i_payload);
			nal_date[i-1].i_payload = pNals[i-1].i_payload;
			#ifndef NDEBUG
			if(3==iNal)
			{
				cout<<"h264 nal test "<<i<<" "<<pNals[i-1].i_payload<<endl;
				cout<<"h264 nal test "<<i<<" "<<nal_date[i-1].i_payload<<endl;
			}
			#endif
			
			nal_date[i-1].i_type = pNals[i-1].i_type;
		}
		mutex_x264_nal_deque.lock();//lock the deque and copy naldate to it 
		x264_nal_deque.push_front(nal_date);
		x264_inal_deque.push_front(iNal);
		mutex_x264_nal_deque.unlock();
		
		
	}else
	{	
		#ifndef NDEBUG
		cout<<"deque_yuv422_date is empty,wait 30ms"<<endl;
		#endif
		ProcessImage::mutex_yuv422_date_deque.unlock();
		msleep(30);
		//sleep;
	}
	
	}//while(1);
	//exec();
}





int H264::convert_yuv422_to_yuv420(unsigned char *pYUV,unsigned char *yuv,unsigned int lWidth,unsigned int lHeight)
{
 
unsigned int i,j;
unsigned char *pY = yuv;
unsigned char *pU = yuv + lWidth*lHeight;
unsigned char *pV = pU + (lWidth*lHeight)/4;
 
unsigned char *pYUVTemp = pYUV;
unsigned char *pYUVTempNext = pYUV+lWidth*2;
         
 for(i=0; i<lHeight; i+=2)
 {
    for(j=0; j<lWidth; j+=2)
     {
       pY[j] = *pYUVTemp++;
       pY[j+lWidth] = *pYUVTempNext++;
                         
       pU[j/2] =(*(pYUVTemp) + *(pYUVTempNext))/2;
       pYUVTemp++;
       pYUVTempNext++;
                         
       pY[j+1] = *pYUVTemp++;
       pY[j+1+lWidth] = *pYUVTempNext++;
                         
       pV[j/2] =(*(pYUVTemp) + *(pYUVTempNext))/2;
       pYUVTemp++;
       pYUVTempNext++;
     }
       pYUVTemp+=lWidth*2;
       pYUVTempNext+=lWidth*2;
       pY+=lWidth*2;
       pU+=lWidth/2;
       pV+=lWidth/2;
   }
         
       return 1;
}





