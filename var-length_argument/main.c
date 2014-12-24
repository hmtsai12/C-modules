#include <stdio.h>
#include <stdarg.h>

/* calculate sum of a 0 terminated list */
void sum(char *msg, ...)
{
	int total = 0;
	va_list ap;
	int arg;
	va_start(ap, msg);
	while ((arg = va_arg(ap, int)) != 0)
	{
		total += arg;
	}
	printf(msg, total);
	va_end(ap);
}

int main(void)
{
	sum("The total of 1+2+3+4 is %d\n", 1,2,3,4,0);
	return 0;
}
