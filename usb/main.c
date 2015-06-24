#include "usb.h"

int main(void)
{
	usb_init();
	usb_find_busses();
	usb_find_devices();
	return 0;
}
