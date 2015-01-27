#include "mbr.h"
#include <stdio.h>
int main(void)
{
	int ret;
	/*unsigned int i;*/
	char modeName[50];
	char serialNumber[100];
	/*unsigned short data[256];*/
	struct ata_identify_device newdata;
	uint64_t hdsize;
	ret = readMBR("/dev/sda", (unsigned short *)&newdata);
	/*ret = readMBR("/dev/sda", data);*/
	if(ret != 0)
	{
		printf("ret = %d\n", ret);
		return 0;
	}
	/*readModeName(modeName, data);
	printf("mode name = %s\n", modeName);
	readSerialNumber(serialNumber, data);
	printf("mode name = %s\n", serialNumber);
	readFwVer(serialNumber, data);
	printf("mode FW = %s\n", serialNumber);*/
	
	copyWordEndianString((unsigned char *)modeName, newdata.model, LENGTH_MODEL);
	stringTrimSpace(modeName);
	printf("mode name = %s\n", modeName);
	copyWordEndianString((unsigned char *)serialNumber, newdata.serial_no, LENGTH_SERIAL);
	stringTrimSpace(serialNumber);
	printf("SerialNumber = %s\n", serialNumber);
	copyWordEndianString((unsigned char *)serialNumber, newdata.fw_rev, LENGTH_FW_REV);
	stringTrimSpace(serialNumber);
	printf("mode FW = %s\n", serialNumber);
	
	hdsize = determine_capacity(&newdata, serialNumber);
	sprintf(modeName, "%lldGbytes", hdsize/1000000000);
	
	printf("size = %s\n",modeName);
	return 0;
}
