#include <stdio.h>
#include <usb.h>

int main(int argc, char *argv[])
{
	struct usb_bus *bus;
	struct usb_device *dev;
	char string[256] = {0};
  
	usb_init();

	if (usb_find_busses() < 0)
		printf("usb_find_busses() failed");
	if (usb_find_devices() < 0)
		printf("usb_find_devices() failed");

	for (bus = usb_get_busses(); bus; bus = bus->next)
	{
		for (dev = bus->devices; dev; dev = dev->next)
		{
			struct usb_device_descriptor *desc;
			struct usb_dev_handle *usb_fp;

			desc = &(dev->descriptor);
			printf("Vendor/Product ID: %04x:%04x \n", desc->idVendor, desc->idProduct);
			if((desc->idVendor == 0x0403) && (desc->idProduct == 0x6010))
			{
			if(!(usb_fp = usb_open(dev)))
				printf("open usb device fail\n");
			if(usb_get_string_simple(usb_fp, dev->descriptor.iProduct, string, sizeof(string)) <= 0)
			{
				printf("unable to fetch product description\n");
			}else
			{
				printf("usb description =%s\n",string);
			}


			if(usb_get_string_simple(usb_fp, dev->descriptor.iSerialNumber, string, sizeof(string)) <= 0)
			{
				printf("unable to fetch serial number\n");
			}else
			{
				printf("usb serial number =%s\n",string);
			}
			}
		}
	}
	return 0;
}
