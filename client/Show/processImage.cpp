#include <QtGui>
#include "processImage.h"
//#include "Show/videodevice.h"
extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}
ProcessImage::ProcessImage(QWidget *parent):QWidget(parent)
{
    //pp = (unsigned char *)malloc(640 * 480/*QWidget::width()*QWidget::height()*/* 3 * sizeof(char));
    frameDate = new Pic_Date(640,480);
    frame = new QImage(frameDate->p_date,640,480,QImage::Format_RGB888);
    label = new QLabel();

    receive = new MyRTPSession();
    decode = new Decode_video();

	//connect(decode,SIGNAL(getFrame()),this,SLOT(update()));
    receive->start();
    decode->start();

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start(10);//单位是ms

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    setLayout(hLayout);
    setWindowTitle(tr("Client"));
}

ProcessImage::~ProcessImage()
{
	receive->quit();
	decode->quit();
	delete frameDate;
	frameDate = NULL;
	delete decode;
	decode = NULL;
	delete receive;
	receive = NULL;
	
	delete frame;
	frame = NULL;
	delete label;
	label = NULL;

}

void ProcessImage::paintEvent(QPaintEvent *)
{
	
	if(!decode->p_frame_deque->isnull())
	{
		
		decode->p_frame_deque->get_frame(frameDate);
		if(frameDate->is_size_changed)
		{
			cout<<"paintEvent :: pic size changed"<<endl;
			delete frame;
			frame = NULL;
			frame = new QImage(frameDate->p_date,640,480,QImage::Format_RGB888);
			this->resize(frameDate->width,frameDate->height);
		}else{
			
		}
		cout<<"paintEvent:: the showing picture is "<<frameDate->i_pt<<endl;
		frame->loadFromData((uchar *)frameDate->p_date,frameDate->width *frameDate->height* 3 * sizeof(char));
		label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
	}else{
		
	}
}


