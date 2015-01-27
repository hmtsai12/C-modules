#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define BUFSIZE 1024
int main(int argc, char *argv[])
{
	int s = 0;
	struct sockaddr_in svr_addr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	int recvlen;
	int rqst;
	unsigned char buf[BUFSIZE];
	fd_set readset;
	int selectRet;
	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("cannot create socket.\n");
		return 0;
	}
	//setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	memset((char *)&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(11111);

	if(bind(s, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0)
	{
		printf("bind failed.\n");
		close(s);
		return 0;
	}

	if(listen(s, 5) < 0)
	{
		printf("listen error\n");
		close(s);
		return 0;
	}
	printf("waiting on port(11111).\n");
	FD_ZERO(&readset);
	FD_SET(s, &readset);
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
				if( (rqst = accept(s, (struct sockaddr *)&remaddr, &addrlen)) < 0)
					printf("connect fail\n");
				else
					printf("got connect\n");

				recvlen = recv(rqst, buf, BUFSIZE, 0);
				if(recvlen < 0)
				{
					perror("recv\n");
				}
				else if(recvlen == 0)
				{
					printf("received close\n");
					close(rqst);
				}
				else
				{
					printf("received %d bytes\n", recvlen);
				}
				if (recvlen > 0)
				{
					buf[recvlen] = 0;
					printf("received message:%s.\n", buf);
					send(rqst, buf, recvlen, 0);
					close(rqst);
				}
			}
			break;
	}
	close(s);
	return 0;
}

