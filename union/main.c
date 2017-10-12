#include <stdio.h>
#include <stdint.h>

struct fun {
	uint32_t b : 24;
	uint32_t c : 4;
	uint32_t d : 4;
};
struct test {
	uint8_t a;
	union utype {
		uint32_t aa;
		struct fun bb;
	} cc;
};

int main(void)
{
	struct test hm;
	hm.a = 50;
	hm.cc.aa = 0xbabbccdd;
	printf("b=0x%x, c=0x%x, d=0x%x\n\r", hm.cc.bb.b, hm.cc.bb.c, hm.cc.bb.d);

	return 0;
}
