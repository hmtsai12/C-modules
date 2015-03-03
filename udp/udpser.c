#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define SERVICE_PORT 11111
#define BUFSIZE 2048
int main(void)
{
	int fd;
	struct sockaddr_in myaddr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	int recvlen;
	int msgcnt = 0;
	unsigned char buf[BUFSIZE] = {0};
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("cannot create socket\n");
		return 0;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);
	
	if(bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
	{
		printf("bind error\n");
		close(fd);
		return 0;
	}
	
	for(;;)
	{
		printf("Waiting on port %d\n", SERVICE_PORT);
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		if(recvlen > 0)
		{
			buf[recvlen] = 0;
			printf("received message: \"%s\" (%d bytes)\n", buf ,recvlen);
		}
		else if(recvlen == 0)
		{
			printf("received close\n");
			printf("total message:%d\n", msgcnt);
			break;
		}
		else
		{
			printf("recvfrom error\n");
		}
		sprintf((char *)buf, "ack %d", msgcnt++);
		printf("sending response \"%s\"\n", buf);
		if(sendto(fd, buf, strlen((char *)buf), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			printf("sendto error\n");
	}
	close(fd);
	return 0;
}


