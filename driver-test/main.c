#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "hello.h"

int main(int argc, char *argv[])
{
	int fp;
	int io_ret;
	hello_transfer data;
	fp = open(argv[2], O_RDWR);
	if(fp < 0)
	{
		printf("open %s error\n", argv[1]);
		return -1;
	}
	if(strcmp(argv[1], "w") == 0)
	{
		data.len = strlen(argv[3]);
		data.tx_buf = (unsigned long)malloc(data.len + 1);
		strcpy((char *)data.tx_buf, argv[3]);
		io_ret = ioctl(fp, HELLO_WRITE, &data);	
		printf("io ret = %d\n", io_ret);
		free((void *)data.tx_buf);
	}
	else if(strcmp(argv[1], "r") == 0)
	{
		data.len = 1024;
		data.rx_buf = (unsigned long)malloc(data.len + 1);
		io_ret = ioctl(fp, HELLO_READ, &data);	
		printf("ret = %d,string=%s\n", data.len, (char *)data.rx_buf);
		printf("io ret = %d\n", io_ret);
		free((void *)data.rx_buf);
	}
	else
		printf("command error\n");
	return 0;
}
