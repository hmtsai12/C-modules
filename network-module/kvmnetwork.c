#include "kvmnetwork.h"	
#include "uart.h"

int openUDPConnectSocket(int port)
{
	int s = 0;
	struct sockaddr_in svr_addr;
	if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("cannot create socket.\n");
		return -1;
	}

	memset((char *)&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(port);

	if(bind(s, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0)
	{
		printf("bind failed.\n");
		close(s);
		return -1;
	}
	return s;
}

int openTCPConnectSocket(int port)
{
	int s = 0;
	struct sockaddr_in svr_addr;
	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("cannot create socket.\n");
		return -1;
	}

	memset((char *)&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(port);

	if(bind(s, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0)
	{
		printf("bind failed.\n");
		close(s);
		return -1;
	}
	
	if(listen(s, 5) < 0)
	{
		perror("listen");
		close(s);
		return -1;
	}
	return s;
}

char processUDPRequest(int s)
{
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	int recvlen;
	char buf[BUFSIZE] = {0};
	int strArgc = 0;
	char *strArgv[5];
	char *retString;

	recvlen = recvfrom(s, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
	printf("received %d bytes\n", recvlen);
	if (recvlen > 0)
	{
		buf[recvlen] = 0;
		printf("received message:%s\n", buf);
	}
	else if(recvlen == 0)
	{
		return -1;
	}
	else
	{
		return -1;
	}
	/*string token*/
	{
		char *delim = ",";
		char *pch;
		pch = strtok(buf, delim);
		printf("string=%s\n",pch);
		if(pch != NULL)
			strArgv[strArgc++] = pch;
		while(pch != NULL)
		{
			pch = strtok(NULL, delim);
			strArgv[strArgc++] = pch;
			printf("string=%s\n",pch);
		}
	}
	
	if(memcmp(strArgv[0], "KVM", 3) == 0)
	{	
		printf("KVM string\n");
		if(uartConnect(MODEMDEVICE) == -1)
		{
			printf("open uart fail\n");
		}
		uartBaudrate(BAUDRATE);
		retString = uartCommand(strArgv[1]);	
		printf("%s", retString);
		printf("len = %d\n", (int)strlen(retString));
		uartDisconnect();
	}	
	else if(memcmp(strArgv[0], "PoChannel", 9) == 0)
	{
		char PoChannelCommand[256] = {0};

		printf("PoChannel string\n");
		snprintf(PoChannelCommand, 255, "echo %s > /tmp/robot_receiver", strArgv[1]);
		if(system(PoChannelCommand) == -1)
			perror("system call error");
	}
	memset(buf, 0x00, BUFSIZE);
	snprintf(buf, BUFSIZE, "%s", retString);
	free(retString);
	if(sendto (s, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen) == -1)
	{
		perror("sendto fail\n");
		return -1;
	}

	return 0;
}

char processTCPRequest(int s)
{
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	int recvlen;
	char buf[BUFSIZE] = {0};
	int strArgc = 0;
	char *strArgv[5];
	char *retString;
	int connectSocket;
	
	if((connectSocket = accept(s, (struct sockaddr *)&remaddr, &addrlen)) < 0)
	{
		perror("accept");
		return -1;
	}

while(1)
{
	recvlen = recv(connectSocket, buf, BUFSIZE, 0);
	
	printf("received %d bytes\n", recvlen);
	if (recvlen > 0)
	{
		buf[recvlen] = 0;
		printf("received message:%s\n", buf);
	}
	else if(recvlen == 0)
	{
		printf("received close message\n");
		close(connectSocket);
		break;
	}
	else
	{
		return -1;
	}
	/*string token*/
	{
		char *delim = ",";
		char *pch;
		pch = strtok(buf, delim);
		printf("string=%s\n",pch);
		if(pch != NULL)
			strArgv[strArgc++] = pch;
		while(pch != NULL)
		{
			pch = strtok(NULL, delim);
			strArgv[strArgc++] = pch;
			printf("string=%s\n",pch);
		}
	}
	
	if(memcmp(strArgv[0], "KVM", 3) == 0)
	{	
		printf("KVM string\n");
		if(uartConnect(MODEMDEVICE) == -1)
		{
			printf("open uart fail\n");
		}
		uartBaudrate(BAUDRATE);
		retString = uartCommand(strArgv[1]);	
		printf("%s", retString);
		printf("len = %d\n", (int)strlen(retString));
		uartDisconnect();
	}	
	else if(memcmp(strArgv[0], "PoChannel", 9) == 0)
	{
		char PoChannelCommand[256] = {0};

		printf("PoChannel string\n");
		snprintf(PoChannelCommand, 255, "echo %s > /tmp/robot_receiver", strArgv[1]);
		if(system(PoChannelCommand) == -1)
			perror("system call error");
	}
	memset(buf, 0x00, BUFSIZE);
	snprintf(buf, BUFSIZE, "%s", retString);
	free(retString);
	if(send (connectSocket, buf, strlen(buf), 0) == -1)
	{
		perror("sendto fail\n");
		return -1;
	}
}
	return 0;
}

void closeTCPConnectSocket(int fd)
{
	close(fd);
}


void closeUDPConnectSocket(int fd)
{
	close(fd);
}
