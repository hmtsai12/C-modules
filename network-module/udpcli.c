#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFLEN 1024
int main(int argc, char **argv)
{
	struct sockaddr_in cli_addr;
	struct sockaddr_in remaddr;
	int s = 0;
	char buf[BUFLEN];
	char *server = "192.168.1.4";
	socklen_t addrlen = sizeof(remaddr);
	int recvlen;
	if((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		printf("open socket fail.\n");
		return 0;
	}

	memset((char *)&cli_addr, 0, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(0);

	if(bind(s, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0 )
	{
		printf("bind failed.\n");
		return 0;
	}

	memset((char *)&remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(11111);

	if(inet_aton(server, &remaddr.sin_addr) == 0)
	{
		printf("inet_aton() failed.\n");
		return 1;
	}

	printf("sending packet to %s port %s\n", server, "11111");
	sprintf(buf, "%s", argv[1]);
	if(sendto (s, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen) == -1)
	{
		printf("sendto fail\n");
		return 1;
	}
	memset(buf, 0x00, BUFLEN);
	recvlen = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&cli_addr, &addrlen);
	printf("received %d bytes\n", recvlen);
	if (recvlen > 0)
	{
		buf[recvlen] = 0;
		printf("received message:%s\n", buf);
	}
	close(s);
	return 0;
}
