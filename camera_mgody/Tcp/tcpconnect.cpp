#include "tcpconnect.h"
#include "state.h"
//init static var in CTcpRobot;

extern int flag_system_run(0);//应答start,控制视频采集的启停。
extern SH264_change h264_change = {0,0,0,0};
extern SCaptrue_change capture_change = {0,0,0,0};


void setSockNonBlock(int sock) 
{
	int flags;
	flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) 
	{
		perror("fcntl(F_GETFL) failed");
		exit(EXIT_FAILURE);
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) 
	{
		perror("fcntl(F_SETFL) failed");
		exit(EXIT_FAILURE);
	}
}
int  CTcpRobot::threadStart()
{


}
void CTcpRobot::threadStop()
{


}
void CTcpRobot::socketInit()
{


}
void CTcpRobot::run()
{
	
	
	
}

