//#include <sys/types.h>
//#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/types.h>
#include <linux/hdreg.h>
#include <sys/ioctl.h>
#include <string.h>
#include "mbr.h"

void stringTrimSpace(char *buf)
{
	unsigned int i = 0, j = 0;
	unsigned char positionFlag = 0;
	unsigned int frontSpaceCount = 0;
	unsigned int rearSpaceCount = 0;
	
	while(buf[i] != 0x00)
	{
		if(buf[i] == 0x20)
		{
			if(positionFlag == 0)
				frontSpaceCount++;
			else
				rearSpaceCount++;
		}
		else
		{
			if(positionFlag == 0)
				positionFlag = 1;
			if(rearSpaceCount != 0)
				rearSpaceCount = 0;
		}
		i++;
	}
	
	if(rearSpaceCount != 0)
		buf[i - rearSpaceCount] = 0x00;

	if(frontSpaceCount != 0)
	{
		for(j = 0; j < (i - frontSpaceCount); j++)
		{
			buf[j] = buf[j + frontSpaceCount];
		}
		buf[(i - frontSpaceCount)] = 0x00;
	}
}

int readMBR(char *deviceName, unsigned short *data)
{
	int fd;
	__u16 *id;
	unsigned char args[4+512] = {WIN_IDENTIFY,0,0,1,};
	unsigned i;
	fd = open(deviceName, O_RDONLY|O_NONBLOCK);
	if(fd < 0)
	{
		printf("Can't open %s\n", deviceName);
		return errno;
	}
		
	if (ioctl(fd, HDIO_DRIVE_CMD, &args))
	{
		args[0] = WIN_PIDENTIFY;
		if (ioctl(fd, HDIO_DRIVE_CMD, &args))
		{
			printf(" HDIO_DRIVE_CMD(identify) failed\n");
			return errno;
		}

	}

	id = (__u16 *)&args[4];
	for (i = 0; i < (256/8); ++i)
	{
		printf("%04x %04x %04x %04x %04x %04x %04x %04x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
		id += 8;
	}
	memcpy((unsigned char *)data, &args[4], 512);
	close(fd);
	return 0;
}

void readModeName(char *name, unsigned short *data)
{
	copyWordEndianString((unsigned char *)name, (unsigned char *)&data[START_MODEL], LENGTH_MODEL);
	stringTrimSpace(name);
}

void readSerialNumber(char *name, unsigned short *data)
{
	copyWordEndianString((unsigned char *)name, (unsigned char *)&data[START_SERIAL], LENGTH_SERIAL);
	stringTrimSpace(name);
}

void readFwVer(char *name, unsigned short *data)
{
	copyWordEndianString((unsigned char *)name, (unsigned char *)&data[START_FW_REV], LENGTH_FW_REV);
	stringTrimSpace(name);
}

void copyWordEndianString( unsigned char * dest , const unsigned char * source , unsigned int len )
{
    char * d = (char*) dest ;
    const char * s = (const char *) source ;
    int i ;
    for( i = 0 ; i < len ; i++ )
    {
        *d++ = s[1] ;
        *d++ = s[0] ;
        s += 2 ;
    }
    *d = 0 ;
}

uint64_t get_num_sectors(struct ata_identify_device *drive)
{
  unsigned short command_set_2  = drive->command_set_2;
  unsigned short capabilities_0 = drive->words047_079[49-47];
  unsigned short sects_16       = drive->words047_079[60-47];
  unsigned short sects_32       = drive->words047_079[61-47];
  unsigned short lba_16         = drive->words088_255[100-88];
  unsigned short lba_32         = drive->words088_255[101-88];
  unsigned short lba_48         = drive->words088_255[102-88];
  unsigned short lba_64         = drive->words088_255[103-88];

  // LBA support?
  if (!(capabilities_0 & 0x0200))
    return 0; // No

  // if drive supports LBA addressing, determine 32-bit LBA capacity
  uint64_t lba32 = (unsigned int)sects_32 << 16 | 
                   (unsigned int)sects_16 << 0  ;

  uint64_t lba64 = 0;
  // if drive supports 48-bit addressing, determine THAT capacity
  if ((command_set_2 & 0xc000) == 0x4000 && (command_set_2 & 0x0400))
      lba64 = (uint64_t)lba_64 << 48 | 
              (uint64_t)lba_48 << 32 |
              (uint64_t)lba_32 << 16 | 
              (uint64_t)lba_16 << 0  ;

  // return the larger of the two possible capacities
  return (lba32 > lba64 ? lba32 : lba64);
}


// This returns the capacity of a disk drive and also prints this into
// a string, using comma separators to make it easier to read.  If the
// drive doesn't support LBA addressing or has no user writable
// sectors (eg, CDROM or DVD) then routine returns zero.
uint64_t determine_capacity(struct ata_identify_device *drive, char *pstring){

  // get correct character to use as thousands separator
  char *separator=",";

  // get #sectors and turn into bytes
  uint64_t capacity = get_num_sectors(drive) * 512;
  uint64_t retval = capacity;

  // print with locale-specific separators (default is comma)
  int started=0, k=1000000000;
  uint64_t power_of_ten = k;
  power_of_ten *= k;
  
  for (k=0; k<7; k++) {
    uint64_t threedigits = capacity/power_of_ten;
    capacity -= threedigits*power_of_ten;
    if (started)
      // we have already printed some digits
      pstring += sprintf(pstring, "%s%03""llu", separator, threedigits);
    else if (threedigits || k==6) {
      // these are the first digits that we are printing
      pstring += sprintf(pstring, "%""llu", threedigits);
      started = 1;
    }
    if (k!=6)
      power_of_ten /= 1000;
  }
  
  return retval;
}
