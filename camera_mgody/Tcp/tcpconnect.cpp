#include "tcpconnect.h"


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

