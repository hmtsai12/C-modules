#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct test {
	uint8_t a;
	uint16_t b;
} TEST;

TEST *list[7] = { NULL };

int main(void)
{
	uint8_t i;
	for (i = 0; i < 7; i++) {
		list[i] = malloc(sizeof(TEST) * 64);
		memset(list[i], 0x00, sizeof(TEST) * 64);
	}
	list[0][3].a = 100;
	list[0][3].b = 1000;
	list[0][2].a = 50;
	list[0][2].b = 500;
	printf("a = %d, b = %d, a = %d, b =%d\n\r", list[0][3].a, list[0][2].b, list[0][2].a, list[0][3].b);
	for (i = 0; i < 7; i++) {
		free(list[i]); 
	}
}
