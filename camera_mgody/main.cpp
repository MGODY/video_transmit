
#include <QtGui>
#include "Qt/processImage.h"
#include <iostream>
using std::cout;
using std::endl;


int main(int argc,char *argv[])
{

	QApplication app(argc,argv);
	ProcessImage* process;
	process = new ProcessImage();
	#ifndef NDEBUG
		process->resize(pic_width,pic_height);
		process->show();
	#endif
	return app.exec();
}
