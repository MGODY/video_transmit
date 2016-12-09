#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include <QtGui>
#include <QThread>
#include <QMutex>
#include <QList>
//#include <QtWidgets>
//#include <QtWidgets/QApplication>
//#include "videodevice.h"
#include "Decode/decode.h"
#include "Accept/accept.h"
#include "ui_client.h"
class ProcessImage : public QMainWindow,public Ui::MainWindow
{
    Q_OBJECT
public:
    ProcessImage(QWidget *parent=0);
    ~ProcessImage();

private:
	
    MyRTPSession* receive;
    Decode_video** decode ;
	Pic_Date** frameDate;
//    QLabel *label;
    QImage **frame;
    QTimer *timer;
   // QHBoxLayout *hLayout 
    //uchar *p_yuv420;
    //uchar *p_yuv422;
	QList<QLabel*> label_list;
private slots:
    void paintEvent(QPaintEvent *);
};



#endif
