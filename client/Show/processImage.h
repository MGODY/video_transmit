#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include <QtGui>
#include <QThread>
#include <QMutex>
//#include <QtWidgets>
//#include <QtWidgets/QApplication>
//#include "videodevice.h"
#include "Decode/decode.h"
#include "Accept/accept.h"
class ProcessImage : public QWidget
{
    Q_OBJECT
public:
    ProcessImage(QWidget *parent=0);
    ~ProcessImage();

private:
    MyRTPSession* receive;
    Decode_video* decode ;
	Pic_Date* frameDate;
    QLabel *label;
    QImage *frame;
    QTimer *timer;
    
    //uchar *p_yuv420;
    //uchar *p_yuv422;

private slots:
    void paintEvent(QPaintEvent *);
};

#endif
