#include <stdio.h>
#include <stdarg.h>
#include <string.h>
void CLI_TMP(unsigned int type, ...);
int main(int argc, char *argv[])
{
	char d[10] = "ABC";

	CLI_TMP(0, 10, 20, 30, d, "ddsfssf", 'c');
	
	return 0;
}

void CLI_TMP(unsigned int type, ...)
{
	va_list argptr;
	char temp_buf[256] = {0};
	int a, b, c;
	char *d, *e;
	char f;

	va_start(argptr, type);

	a = va_arg(argptr, int);
	b = va_arg(argptr, int);
	c = va_arg(argptr, int);
	d = va_arg(argptr, char *);
	e = va_arg(argptr, char *);
	f = va_arg(argptr, int);

	strcat(d, "aaa");
	va_end(argptr);

	printf("%d, %d, %d, %s, %s, %c\n", a, b, c, d, e, f);
}
