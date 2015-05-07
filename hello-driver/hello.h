#ifndef _HELLO_H_
#define _HELLO_H_

#include <linux/ioctl.h>
typedef struct
{
	int len;
	unsigned long tx_buf;
	unsigned long rx_buf;
}hello_transfer;

#define HELLO_DEVNAME "/dev/hello"

#define HELLO_IOC_MAGIC 'a'
#define HELLO_WRITE		_IOWR(HELLO_IOC_MAGIC, 1, hello_transfer)
#define HELLO_READ		_IOWR(HELLO_IOC_MAGIC, 2, hello_transfer)
#define HELLO_ENABLE		_IO(HELLO_IOC_MAGIC, 3)
#define HELLO_DISABLE		_IO(HELLO_IOC_MAGIC, 4)
#define HELLO_IOC_MAXNR	4	

#endif /*_HELLO_H_*/
