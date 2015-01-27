#include <stdio.h>
#include <string.h>

#include "uart.h"
#include "kvmnetwork.h"

int main(int argc, char *argv[])
{
	int s = 0;
	char ret;
	fd_set readset;

	s = openTCPConnectSocket(11111);
	if( s == -1)
	{
		printf("open udp socket fail\n");
		return -1;
	}
	
	FD_ZERO(&readset);
	FD_SET(s, &readset);	
	while(1)
	{
		int selectRet;
		
		selectRet = select(FD_SETSIZE, &readset, NULL, NULL, NULL);

		switch(selectRet)
		{
			case 0:
				printf("select timeout\n");
				break;
			case -1:
				perror("select");
				break;
			default:
				if(FD_ISSET(s, &readset))
				{
					ret = processTCPRequest(s);
					if(ret == -1)
					{
						printf("UDP error\n");
					}
				}
				break;
		}
	}
	closeTCPConnectSocket(s);
	return 0;
}

