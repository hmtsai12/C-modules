#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "uart.h"

#define BAUDRATE B115200
#define MODEMDEVICE	"/dev/ttyUSB0"

int fd;
struct termios oldtio, newtio;

char uartConnect(char* port)
{
	
	fd = open(port, O_RDWR|O_NOCTTY|O_NONBLOCK);
	//fd = open(MODEMDEVICE, O_RDWR|O_NOCTTY);
	//fd = open(argv[1], O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd < 0)
	{
		perror(port);
		return -1;
	}
	return 0;
}

void uartBaudrate(speed_t speed)
{
	tcgetattr(fd, &oldtio);

	bzero(&newtio, sizeof(newtio));

	newtio.c_cflag = speed|CS8|CLOCAL|CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	newtio.c_lflag = ICANON;

	tcflush(fd, TCIFLUSH);

	tcsetattr(fd, TCSANOW, &newtio);
}

char *uartCommand(char *command)
{
	static char s1[20] = {0};
	int res;
	char *buffer = NULL;
	int retLen = 0;
	char buf[256] = {0};
	memcpy(s1, command, strlen(command));
	s1[strlen(command)] = 10;
	s1[strlen(command)+1] = 13;
	res = write(fd, s1, strlen(s1));
	
	sleep(1);
	buffer = (char *)malloc(sizeof(char)*1);
	buffer[0] = 0;
	retLen = 1;
	while(1)
	{
		res = read(fd, buf, 255);
		if(res == -1) break;/*no more string*/
		buf[res]=0;
		retLen = retLen + res;
		buffer = (char *)realloc((void *)buffer, (sizeof(char)*retLen));
		if(buffer == NULL)
		{
			perror("alloc fail");
			return NULL;
		}
		strcat(buffer, buf);
		if(buf[0] == 0) break;
	}
	return buffer;

}
void uartDisconnect()
{
	close(fd);
	tcsetattr(fd, TCSANOW, &oldtio);
}
int main(int argc, char *argv[])
{
	char *retString;
	if(argc != 2)
	{
		printf("input command\n");
		exit(1);
	}

	if(uartConnect(MODEMDEVICE) == -1)
	{
		return -1;
	}
	uartBaudrate(BAUDRATE);
	retString = uartCommand(argv[1]);	
	printf("ret String\n");
	printf("%s", retString);
	printf("len = %d\n", (int)strlen(retString));
	free(retString);
	uartDisconnect();
	return 0;
}
