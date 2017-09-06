#include <stdio.h>
#include <stdint.h>
#include "dh1cf.h"

int main()
{
	int channels = 129;
	unsigned char MACAddr[8] = { 0x00, 0x13, 0x50, 0x04, 0x00, 0x00, 0x05, 0xf8};
	unsigned char MACAddr1[8] = { 0x00, 0x13, 0x50, 0x04, 0x00, 0x00, 0x06, 0x02};
	int getIndex;

	uint16_t slot;

	printf("unicast\n\r");
	for(slot = 10738; slot <= 10747; slot++)
	{
		getIndex = getUnicastChannelIndex(slot, MACAddr, channels);
		printf("slot[%d] = %d\n\r", slot, getIndex);
	}
	printf("unicast1\n\r");
	for(slot = 16446; slot <= 16455; slot++)
	{
		getIndex = getUnicastChannelIndex(slot, MACAddr1, channels);
		printf("slot[%d] = %d\n\r", slot, getIndex);
	}
	return 0;
}
