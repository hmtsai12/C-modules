#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define BUFLEN 2048
#define SERVICE_PORT 11111
#define MSGS 5
int main(void)
{
	int fd, i;
	struct sockaddr_in myaddr, remaddr;
	socklen_t slen = sizeof(remaddr);
	char buf[BUFLEN];
	int recvlen;
	char *server = "127.0.0.1";

	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		printf("create socket fail\n");
		return 0;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if(bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
	{
		printf("bind error\n");
		close(fd);
		return 0;
	}

	memset((char *)&remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if(inet_aton(server, &remaddr.sin_addr) == 0)
	{
		fprintf(stderr, "inet_aton() fail\n");
		close(fd);
		return 0;
	}
	
	for(i = 0; i < MSGS; i++)
	{
		printf("sending packet %d to %s port %d\n", i, server, SERVICE_PORT);
		sprintf(buf, "This is packet %d", i);
		if(sendto (fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen) == -1)
		{
			printf("sendto error\n");
			close(fd);
			return 0;
		}
		recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
		if(recvlen >= 0)
		{
			buf[recvlen] = 0;
			printf("received message: \%s\"\n", buf);
		}
	}

	close(fd);
	return 0;
}
