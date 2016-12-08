#ifndef tcpconnect.h
#define tcpconnect_h
/*-----------------------------------------------
* 功能 ： 用于tcp连接服务器
* author ： 冯坤 (mgody.fengkun@gmail.com) 
* time : first @ 2016-12-04 , finished @
* version 1.0
* ISST 系统科学与技术研究所
-------------------------------------------------*/
#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <string.h>  

#include <sys/types.h>  
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <sys/wait.h>  
#include <sys/time.h>
#include <fcntl.h>
#include <arpa/inet.h>  
#include <unistd.h>  

#include <string>
#include <iostream>

//#include <QObject>
#include <QThread>
#include <QMutex>
#include "mesegtype.h"
void setSockNonBlock(int );
//需要根据客户端更改的参数有：1、分辨率；2、码率 3、帧率 
//
class CTcpRobot :QThread
{
	QObject
public :
	friend void setSockNonBlock(int ); 
	static int runflag;
private:
	int sock;
	QMutex mutex_sock;
	
	
private:
	int sendstates();
};















#endif 

