#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define RPL_LOLLIPOP_CIRCULAR_REGION 65535
#define RPL_LOLLIPOP_SEQUENCE_WINDOWS 5

#define lollipop_greater_than(a, b)\
	((a > b && (a - b) <= RPL_LOLLIPOP_SEQUENCE_WINDOWS) || (a < b && (b - a) >= (RPL_LOLLIPOP_CIRCULAR_REGION + 1 - RPL_LOLLIPOP_SEQUENCE_WINDOWS)))
	

	
int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("error\n");
		return -1;
	}
	uint16_t a = atoi(argv[1]);
	uint16_t b = atoi(argv[2]);
	printf("%s, %s, %s\n", argv[0], argv[1], argv[2]);
	printf("result=%d\n", lollipop_greater_than(a, b));
	printf("result2=%d\n", lollipop_greater_than(b, a));
	return 0;
}
