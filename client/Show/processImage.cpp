#include <QtGui>
#include "processImage.h"
//#include "Show/videodevice.h"
extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}
ProcessImage::ProcessImage(QWidget *parent):QMainWindow(parent)
{
	setupUi(this);
    receive = new MyRTPSession();

    frameDate = new Pic_Date* [Video_MaxNum];
    decode = new Decode_video* [Video_MaxNum];
    frame = new QImage* [Video_MaxNum];
    label_list = this->findChildren<QLabel*>();
    
	for(int i = 1;i<=Video_MaxNum;i++)
	{//创建Video_MaxNum个解码器对象以及Decode_video对象，每个对象的初值是决定其从哪个rtp数据包队列中读取数据进行解码。
		decode[i-1] = new Decode_video(i-1);
		frameDate[i-1] = new Pic_Date(640,480);
		frame[i-1] = new QImage(frameDate[i-1]->p_date,640,480,QImage::Format_RGB888);
	}
	//connect(decode,SIGNAL(getFrame()),this,SLOT(update()));
    receive->start();
    for(int i = 1;i<=Video_MaxNum;i++)
    {
    	decode[i-1]->start();
   	}
   	
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start(10);//单位是ms

    //hLayout = new QHBoxLayout();
   // hLayout->addWidget(label);
   // setLayout(hLayout);
   // setWindowTitle(tr("Client"));
}

ProcessImage::~ProcessImage()
{
	receive->quit();
	//decode->quit();

	delete receive;
	receive = NULL;

	for(int i = 1;i<=Video_MaxNum;i++)
	{//创建Video_MaxNum个解码器对象以及Decode_video对象，每个对象的初值是决定其从哪个rtp数据包队列中读取数据进行解码。
		delete frameDate[i-1];
		frameDate[i-1] = NULL;
		delete decode[i-1];
		decode[i-1] = NULL;
		delete frame[i-1];
		frame[i-1] = NULL;
	}
	delete decode;
	decode = NULL;
	delete frameDate;
	frameDate = NULL;
}

void ProcessImage::paintEvent(QPaintEvent *)
{
	for(int i = 1;i<=Video_MaxNum;i++)
	{
		if(!decode[i-1]->p_frame_deque->isnull())
		{
			
			decode[i-1]->p_frame_deque->get_frame(frameDate[i-1]);
			if(frameDate[i-1]->is_size_changed)
			{
				cout<<"paintEvent :: pic size changed"<<endl;
				delete frame[i-1];
				frame[i-1] = NULL;
				frame[i-1] = new QImage(frameDate[i-1]->p_date,frameDate[i-1]->width,frameDate[i-1]->height,QImage::Format_RGB888);
				//this->resize(frameDate->width,frameDate->height);
				//label_1->resize(frameDate->width,frameDate->height);
	
			}else{
				
			}
			cout<<"paintEvent:: the showing picture is the"<<i-1<<"decoder  "<<frameDate[i-1]->i_pt<<endl;
			frame[i-1]->loadFromData((uchar *)frameDate[i-1]->p_date,frameDate[i-1]->width *frameDate[i-1]->height* 3 * sizeof(char));

			label_list[i-1]->setPixmap(QPixmap::fromImage(*frame[i-1],Qt::AutoColor));
		}else{
			
		}
	}
}


