#include<Qt/processImage.h>

deque<uchar* > ProcessImage::deque_yuv422_date;
QMutex ProcessImage::mutex_yuv422_date_deque;
#ifndef NDEBUG
ProcessImage::ProcessImage(QWidget *parent):QWidget(parent)
#else
ProcessImage::ProcessImage()
#endif
{
	int rs_open,rs_init,rs_capture;
    pp = (unsigned char *)malloc(pic_width * pic_height/*QWidget::width()*QWidget::height()*/* 3 * sizeof(char));
    frame = new QImage(pp,pic_width,pic_height,QImage::Format_RGB888);
    label = new QLabel();
    
    
    vd  = new VideoDevice(tr("/dev/video0"));
    con = new H264();
    rtp = new RtpSender();



    con->h264_init();

	rtp->add_address(Destip,Destport);
    cout<<"init ok"<<endl;
    rs_open = vd->open_device();
    if(-1==rs_open)
    {
        //QMessageBox::warning(this,tr("error"),tr("open /dev/dsp error"),QMessageBox::Yes);
        cout<<"设备打开失败，请检查摄像头是否连接正常"<<endl;
        vd->close_device();
    }else
    {

		rs_init = vd->init_device();
		if(-1==rs_init)
		{
	       // QMessageBox::warning(this,tr("error"),tr("init failed"),QMessageBox::Yes);
			cout<<"设备初始化失败，请检查摄像头是否连接正常"<<endl;
			vd->close_device();
		}else
		{
			rs_capture = vd->start_capture();
			if(-1==rs_capture)
			{
		        //QMessageBox::warning(this,tr("error"),tr("get frame failed"),QMessageBox::Yes);
				cout<<"无法获得摄像头流数据，请检查摄像头是否连接正常"<<endl;
				vd->stop_capture();
			}else
			{
				cout<<"摄像头初始化成功！！！"<<endl;
			}
		}
    }
    
#ifndef NDEBUG
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    //timer->start(reflash_time);
 
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    setLayout(hLayout);
    setWindowTitle(tr("bysj"));

#else
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(Maincirc()));
    //timer->start(reflash_time);
#endif
	if(rs_open||rs_init||rs_capture)
	{
		cout<<"无法开启摄像头，请检查设备是否连接好，然后重新打开程序!!!"<<endl;
	}else
	{
		timer->start(reflash_time);
		con->start();
		rtp->start();
		cout<<"程序初始化成功，开始采集图像并传输"<<endl;
	}

}

 ProcessImage::~ProcessImage()
{
    rs = vd->stop_capture();
    rs = vd->unmap();
    rs = vd->close_device();
    
    delete vd;
    vd = NULL;
    delete con;
    con = NULL;
    delete rtp;
    rtp = NULL;
    free(pp);
    pp=NULL;
    
//    if(!deque_yuv422_date.empty())
//    {
//    auto a=deque_yuv422_date.begin();
//    auto b=deque_yuv422_date.end();
//
//		}
}




#ifndef NDEBUG // 用来选择是否开启本地显示界面， 如果要远程调试，一定得定义NDEBUG，不能开启DEBUG模式
void ProcessImage::paintEvent(QPaintEvent *)
{   
    
    
    rs = vd->get_frame((void**)&p,&len);
    
    //emit convert(p,pic_width ,pic_height);
    push_frame_date(p);
     
    //con->quit(); 
    //while(!go_on_signal);
   // go_on_signal=0;
    convert_yuv_to_rgb_buffer(p,pp,pic_width,pic_height/*QWidget::width(),QWidget::height()*/);
    frame->loadFromData((uchar *)pp,/*len*/pic_width * pic_height * 3 * sizeof(char));
    

 
    label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));

    rs = vd->go_on_frame();
    
    //rtp->start();
}
#else
void ProcessImage::Maincirc()
{   

    rs = vd->get_frame((void**)&p,&len);
    push_frame_date(p);
    rs = vd->go_on_frame();

}
#endif
int ProcessImage::convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
   unsigned int in, out = 0;
   unsigned int pixel_16;
   unsigned char pixel_24[3];
   unsigned int pixel32;
   int y0, u, y1, v;
   for(in = 0; in < width * height * 2; in += 4) {
   pixel_16 =
   yuv[in + 3] << 24 |
   yuv[in + 2] << 16 |
   yuv[in + 1] <<  8 |
   yuv[in + 0];
   y0 = (pixel_16 & 0x000000ff);
   u  = (pixel_16 & 0x0000ff00) >>  8;
   y1 = (pixel_16 & 0x00ff0000) >> 16;
   v  = (pixel_16 & 0xff000000) >> 24;
   pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
   pixel_24[0] = (pixel32 & 0x000000ff);
   pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
   pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
   rgb[out++] = pixel_24[0];
   rgb[out++] = pixel_24[1];
   rgb[out++] = pixel_24[2];
   pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
   pixel_24[0] = (pixel32 & 0x000000ff);
   pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
   pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
   rgb[out++] = pixel_24[0];
   rgb[out++] = pixel_24[1];
   rgb[out++] = pixel_24[2];
 }
 return 0;
}
int ProcessImage::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
	 unsigned int pixel32 = 0;
	 unsigned char *pixel = (unsigned char *)&pixel32;
	 int r, g, b;
	 r = y + (1.370705 * (v-128));
	 g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
	 b = y + (1.732446 * (u-128));
	 if(r > 255) r = 255;
	 if(g > 255) g = 255;
	 if(b > 255) b = 255;
	 if(r < 0) r = 0;
	 if(g < 0) g = 0;
	 if(b < 0) b = 0;
	 pixel[0] = r * 220 / 256;
	 pixel[1] = g * 220 / 256;
	 pixel[2] = b * 220 / 256;
	 return pixel32;
}


	int ProcessImage::push_frame_date(uchar * p_yuv422date)
	{
		unsigned char *p_getyuv422date;
		p_getyuv422date = (unsigned char *)malloc(pic_width * pic_height/*QWidget::width()*QWidget::height()*/* 2 * sizeof(unsigned char));
		if(p_getyuv422date!=NULL)
		{
			memcpy(p_getyuv422date,p_yuv422date,pic_width * pic_height/*QWidget::width()*QWidget::height()*/* 2);
			
		}else
		{
			#ifndef NDEBUG
			cout<<"p_getyuv422date is empty!!"<<endl;
			#endif
		}
		mutex_yuv422_date_deque.lock();
		if (deque_yuv422_date.size()<=30)
		{
			deque_yuv422_date.push_front(p_getyuv422date);
		}else
		{
			
			free(deque_yuv422_date.back());
			deque_yuv422_date.back() = NULL;
			deque_yuv422_date.pop_back();
			deque_yuv422_date.push_front(p_getyuv422date);
			#ifndef NDEBUG
			cout<<"deque_yuv422_date is over 30 frames"<<endl;
			#endif
		}
		mutex_yuv422_date_deque.unlock();
		return 0;
	}

