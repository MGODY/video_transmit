#include <tcpserver.h>
#include "rtpt.h"

int main(int argc,char *argv[])
{

    QApplication app(argc,argv);
    CTcpserv *tcpserver;
    tcpserver = new CTcpserv;
    tcpserver->start();
	return app.exec();

}
