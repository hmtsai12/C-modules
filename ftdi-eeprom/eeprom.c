#include <stdio.h>
#include <string.h>
//#include "eeprom.h"
#include "ftdi.h"

#define EEPROM_SIZE 256 
#define MAC_SIZE 12
#define VENDOR_ID 0x403
#define PRODUCT_ID 0x6010
#define PRODUCT232H_ID 0x6014

int ftdi_232h_eeprom_build(struct ftdi_eeprom *eeprom, unsigned char *output)
{
	unsigned char i, j, eeprom_size_mask;
	unsigned short checksum, value;
	unsigned char manufacturer_size = 0, product_size = 0, serial_size = 0;
	int size_check;
	
	const int cbus_max[5] = {13, 13, 13, 13, 9};
	if (eeprom == NULL)
		return -2;

	if (eeprom->manufacturer != NULL)
		manufacturer_size = strlen(eeprom->manufacturer);
	if (eeprom->product != NULL)
		product_size = strlen(eeprom->product);
	if (eeprom->serial != NULL)
		serial_size = strlen(eeprom->serial);
	
	// highest allowed cbus value
	for (i = 0; i < 5; i++)
	{
		if ((eeprom->cbus_function[i] > cbus_max[i]) ||
				(eeprom->cbus_function[i] && eeprom->chip_type != TYPE_R)) return -3;
	}
	
	if (eeprom->chip_type != TYPE_R)
	{
		if (eeprom->invert) return -4;
		if (eeprom->high_current) return -5;
	}
	
	//size_check = eeprom->size;
	size_check = 80; // 28 are always in use (fixed)

	// Top half of a 256byte eeprom is used just for strings and checksum
	// it seems that the FTDI chip will not read these strings from the lower half
	// Each string starts with two bytes; offset and type (0x03 for string)
	// the checksum needs two bytes, so without the string data that 8 bytes from the top half
	//if (eeprom->size>=256) size_check = 120;
	
	size_check -= manufacturer_size*2;
	size_check -= product_size*2;
	size_check -= serial_size*2;
	// eeprom size exceeded?
	
	if (size_check < 0)
		return (-1);
	
	// empty eeprom
	memset (output, 0, eeprom->size);

	// Addr 00: High current IO
	//output[0x00] = eeprom->high_current ? HIGH_CURRENT_DRIVE : 0;
	// Addr 01: IN endpoint size (for R type devices, different for FT2232)

	//if (eeprom->chip_type == TYPE_R) {
		//output[0x01] = 0x40;
	//}
	//fix 0x00 and 0x01
	output[0x00] = 0x10;
	output[0x01] = 0x00;
	// Addr 02: Vendor ID
	output[0x02] = eeprom->vendor_id;
	output[0x03] = eeprom->vendor_id >> 8;
	
	// Addr 04: Product ID
	output[0x04] = eeprom->product_id;
	output[0x05] = eeprom->product_id >> 8;
	
	// Addr 06: Device release number (0400h for BM features)
	output[0x06] = 0x00;
	switch (eeprom->chip_type) {
		case TYPE_AM:
			output[0x07] = 0x02;
			break;
		case TYPE_BM:
			output[0x07] = 0x04;
			break;
		case TYPE_2232C:
			output[0x07] = 0x05;
			break;
		case TYPE_R:
			output[0x07] = 0x06;
			break;
		case TYPE_2232H:
			output[0x07] = 0x07;
			break;
		case TYPE_4232H:
			output[0x07] = 0x08;
			break;
		case TYPE_232H:
			output[0x07] = 0x09;
			break;
		default:
			output[0x07] = 0x00;
	}

	// Addr 08: Config descriptor
	// Bit 7: always 1
	// Bit 6: 1 if this device is self powered, 0 if bus powered
	// Bit 5: 1 if this device uses remote wakeup
	// Bit 4: 1 if this device is battery powered
	j = 0x80;
	if (eeprom->self_powered == 1)
		j |= 0x40;
	if (eeprom->remote_wakeup == 1)
		j |= 0x20;
	output[0x08] = j;
	
	// Addr 09: Max power consumption: max power = value * 2 mA
	output[0x09] = eeprom->max_power;
	
	// Addr 0A: Chip configuration
	// Bit 7: 0 - reserved
	// Bit 6: 0 - reserved
	// Bit 5: 0 - reserved
	// Bit 4: 1 - Change USB version
	// Bit 3: 1 - Use the serial number string
	// Bit 2: 1 - Enable suspend pull downs for lower power
	// Bit 1: 1 - Out EndPoint is Isochronous
	// Bit 0: 1 - In EndPoint is Isochronous
	//
	j = 0;
	if (eeprom->in_is_isochronous == 1)
		j = j | 1;
	if (eeprom->out_is_isochronous == 1)
		j = j | 2;
	if (eeprom->suspend_pull_downs == 1)
		j = j | 4;
	if (eeprom->use_serial == 1)
		j = j | 8;
	if (eeprom->change_usb_version == 1)
		j = j | 16;
	output[0x0A] = j;
	
	// Addr 0B: Invert data lines
	output[0x0B] = eeprom->invert & 0xff;
	
	// Addr 0C: USB version low byte when 0x0A bit 4 is set
	// Addr 0D: USB version high byte when 0x0A bit 4 is set
	if (eeprom->change_usb_version == 1)
	{
	output[0x0C] = eeprom->usb_version;
	output[0x0D] = eeprom->usb_version >> 8;
	}
	
	//output[0x0C] = 0x01;
	//output[0x0D] = 0x01;
	
	// Addr 0E: Offset of the manufacturer string + 0x80, calculated later
	// Addr 0F: Length of manufacturer string
	output[0x0F] = manufacturer_size*2 + 2;
	
	// Addr 10: Offset of the product string + 0x80, calculated later
	// Addr 11: Length of product string
	output[0x11] = product_size*2 + 2;
	
	// Addr 12: Offset of the serial string + 0x80, calculated later
	// Addr 13: Length of serial string
	output[0x13] = serial_size*2 + 2;
	
	// Addr 14: CBUS function: CBUS0, CBUS1
	// Addr 15: CBUS function: CBUS2, CBUS3
	// Addr 16: CBUS function: CBUS5
	output[0x14] = eeprom->cbus_function[0] | (eeprom->cbus_function[1] << 4);
	output[0x15] = eeprom->cbus_function[2] | (eeprom->cbus_function[3] << 4);
	output[0x16] = eeprom->cbus_function[4];
	output[0x1e] = 0x66; /*eeprom type 0x46 for 93xx46, 0x56 for 93xx56, 0x66 for 93xx66*/
	// Addr 17: Unknown
	
	// Dynamic content
	// In images produced by FTDI's FT_Prog for FT232R strings start at 0x18
	// Space till 0x18 should be considered as reserved.
	//if (eeprom->chip_type >= TYPE_R) {
	//	i = 0x18;
	//} else {
	//	i = 0x14;
	//}
	//if (eeprom->size >= 256) i = 0x80;
	
	/* Wrap around 0x80 for 128 byte EEPROMS (Internale and 93x46) */
	eeprom_size_mask = eeprom->size - 1;
	/*for 2232H*/
	i = 0xA0;
	output[0x8a] = 0x48;	
	// Output manufacturer
	output[0x0E] = i;  // calculate offset
	output[i & eeprom_size_mask] = manufacturer_size*2 + 2, i++;
	output[i & eeprom_size_mask] = 0x03, i++; // type: string
	for (j = 0; j < manufacturer_size; j++)
	{
		output[i & eeprom_size_mask] = eeprom->manufacturer[j], i++;
		output[i & eeprom_size_mask] = 0x00, i++;
	}
	// Output product name
	output[0x10] = i | 0x80;  // calculate offset
	output[i & eeprom_size_mask] = product_size*2 + 2, i++;
	output[i & eeprom_size_mask] = 0x03, i++;
	for (j = 0; j < product_size; j++)
	{
		output[i & eeprom_size_mask] = eeprom->product[j], i++;
		output[i & eeprom_size_mask] = 0x00, i++;
	}
	
	// Output serial
	output[0x12] = i | 0x80; // calculate offset
	output[i & eeprom_size_mask] = serial_size*2 + 2, i++;
	output[i & eeprom_size_mask] = 0x03, i++;
	for (j = 0; j < serial_size; j++)
	{
		output[i & eeprom_size_mask] = eeprom->serial[j], i++;
		output[i & eeprom_size_mask] = 0x00, i++;
	}
	
	// calculate checksum
	checksum = 0xAAAA;
	
	for (i = 0; i < eeprom->size/2-1; i++)
	{
		value = output[i*2];
		value += output[(i*2)+1] << 8;
	
		checksum = value^checksum;
		checksum = (checksum << 1) | (checksum >> 15);
	}
	
	output[eeprom->size-2] = checksum;
	output[eeprom->size-1] = checksum >> 8;
	return size_check;
}

int ftdi_2232h_eeprom_build(struct ftdi_eeprom *eeprom, unsigned char *output)
{
	unsigned char i, j, eeprom_size_mask;
	unsigned short checksum, value;
	unsigned char manufacturer_size = 0, product_size = 0, serial_size = 0;
	int size_check;
	
	const int cbus_max[5] = {13, 13, 13, 13, 9};
	if (eeprom == NULL)
		return -2;

	if (eeprom->manufacturer != NULL)
		manufacturer_size = strlen(eeprom->manufacturer);
	if (eeprom->product != NULL)
		product_size = strlen(eeprom->product);
	if (eeprom->serial != NULL)
		serial_size = strlen(eeprom->serial);
	
	// highest allowed cbus value
	for (i = 0; i < 5; i++)
	{
		if ((eeprom->cbus_function[i] > cbus_max[i]) ||
				(eeprom->cbus_function[i] && eeprom->chip_type != TYPE_R)) return -3;
	}
	
	if (eeprom->chip_type != TYPE_R)
	{
		if (eeprom->invert) return -4;
		if (eeprom->high_current) return -5;
	}
	
	size_check = eeprom->size;
	size_check -= 28; // 28 are always in use (fixed)

	// Top half of a 256byte eeprom is used just for strings and checksum
	// it seems that the FTDI chip will not read these strings from the lower half
	// Each string starts with two bytes; offset and type (0x03 for string)
	// the checksum needs two bytes, so without the string data that 8 bytes from the top half
	if (eeprom->size>=256) size_check = 120;
	
	size_check -= manufacturer_size*2;
	size_check -= product_size*2;
	size_check -= serial_size*2;
	// eeprom size exceeded?
	
	if (size_check < 0)
		return (-1);
	
	// empty eeprom
	memset (output, 0, eeprom->size);

	// Addr 00: High current IO
	//output[0x00] = eeprom->high_current ? HIGH_CURRENT_DRIVE : 0;
	// Addr 01: IN endpoint size (for R type devices, different for FT2232)

	//if (eeprom->chip_type == TYPE_R) {
		//output[0x01] = 0x40;
	//}
	//fix 0x00 and 0x01
	output[0x00] = 0x8;
	output[0x01] = 0x8;
	// Addr 02: Vendor ID
	output[0x02] = eeprom->vendor_id;
	output[0x03] = eeprom->vendor_id >> 8;
	
	// Addr 04: Product ID
	output[0x04] = eeprom->product_id;
	output[0x05] = eeprom->product_id >> 8;
	
	// Addr 06: Device release number (0400h for BM features)
	output[0x06] = 0x00;
	switch (eeprom->chip_type) {
		case TYPE_AM:
			output[0x07] = 0x02;
			break;
		case TYPE_BM:
			output[0x07] = 0x04;
			break;
		case TYPE_2232C:
			output[0x07] = 0x05;
			break;
		case TYPE_R:
			output[0x07] = 0x06;
			break;
		case TYPE_2232H:
			output[0x07] = 0x07;
			break;
		case TYPE_4232H:
			output[0x07] = 0x08;
			break;
		case TYPE_232H:
			output[0x07] = 0x09;
			break;
		default:
			output[0x07] = 0x00;
	}

	// Addr 08: Config descriptor
	// Bit 7: always 1
	// Bit 6: 1 if this device is self powered, 0 if bus powered
	// Bit 5: 1 if this device uses remote wakeup
	// Bit 4: 1 if this device is battery powered
	j = 0x80;
	if (eeprom->self_powered == 1)
		j |= 0x40;
	if (eeprom->remote_wakeup == 1)
		j |= 0x20;
	output[0x08] = j;
	
	// Addr 09: Max power consumption: max power = value * 2 mA
	output[0x09] = eeprom->max_power;
	
	// Addr 0A: Chip configuration
	// Bit 7: 0 - reserved
	// Bit 6: 0 - reserved
	// Bit 5: 0 - reserved
	// Bit 4: 1 - Change USB version
	// Bit 3: 1 - Use the serial number string
	// Bit 2: 1 - Enable suspend pull downs for lower power
	// Bit 1: 1 - Out EndPoint is Isochronous
	// Bit 0: 1 - In EndPoint is Isochronous
	//
	j = 0;
	if (eeprom->in_is_isochronous == 1)
		j = j | 1;
	if (eeprom->out_is_isochronous == 1)
		j = j | 2;
	if (eeprom->suspend_pull_downs == 1)
		j = j | 4;
	if (eeprom->use_serial == 1)
		j = j | 8;
	if (eeprom->change_usb_version == 1)
		j = j | 16;
	output[0x0A] = j;
	
	// Addr 0B: Invert data lines
	output[0x0B] = eeprom->invert & 0xff;
	
	// Addr 0C: USB version low byte when 0x0A bit 4 is set
	// Addr 0D: USB version high byte when 0x0A bit 4 is set
	if (eeprom->change_usb_version == 1)
	{
	output[0x0C] = eeprom->usb_version;
	output[0x0D] = eeprom->usb_version >> 8;
	}
	
	
	// Addr 0E: Offset of the manufacturer string + 0x80, calculated later
	// Addr 0F: Length of manufacturer string
	output[0x0F] = manufacturer_size*2 + 2;
	
	// Addr 10: Offset of the product string + 0x80, calculated later
	// Addr 11: Length of product string
	output[0x11] = product_size*2 + 2;
	
	// Addr 12: Offset of the serial string + 0x80, calculated later
	// Addr 13: Length of serial string
	output[0x13] = serial_size*2 + 2;
	
	// Addr 14: CBUS function: CBUS0, CBUS1
	// Addr 15: CBUS function: CBUS2, CBUS3
	// Addr 16: CBUS function: CBUS5
	output[0x14] = eeprom->cbus_function[0] | (eeprom->cbus_function[1] << 4);
	output[0x15] = eeprom->cbus_function[2] | (eeprom->cbus_function[3] << 4);
	output[0x16] = eeprom->cbus_function[4];
	output[0x18] = 0x46; /*eeprom type 0x46 for 93xx46, 0x56 for 93xx56, 0x66 for 93xx66*/
	// Addr 17: Unknown
	
	// Dynamic content
	// In images produced by FTDI's FT_Prog for FT232R strings start at 0x18
	// Space till 0x18 should be considered as reserved.
	//if (eeprom->chip_type >= TYPE_R) {
	//	i = 0x18;
	//} else {
	//	i = 0x14;
	//}
	//if (eeprom->size >= 256) i = 0x80;
	
	/* Wrap around 0x80 for 128 byte EEPROMS (Internale and 93x46) */
	eeprom_size_mask = eeprom->size - 1;
	/*for 2232H*/
	i = 0x9A;
	
	// Output manufacturer
	output[0x0E] = i;  // calculate offset
	output[i & eeprom_size_mask] = manufacturer_size*2 + 2, i++;
	output[i & eeprom_size_mask] = 0x03, i++; // type: string
	for (j = 0; j < manufacturer_size; j++)
	{
		output[i & eeprom_size_mask] = eeprom->manufacturer[j], i++;
		output[i & eeprom_size_mask] = 0x00, i++;
	}
	// Output product name
	output[0x10] = i | 0x80;  // calculate offset
	output[i & eeprom_size_mask] = product_size*2 + 2, i++;
	output[i & eeprom_size_mask] = 0x03, i++;
	for (j = 0; j < product_size; j++)
	{
		output[i & eeprom_size_mask] = eeprom->product[j], i++;
		output[i & eeprom_size_mask] = 0x00, i++;
	}
	
	// Output serial
	output[0x12] = i | 0x80; // calculate offset
	output[i & eeprom_size_mask] = serial_size*2 + 2, i++;
	output[i & eeprom_size_mask] = 0x03, i++;
	for (j = 0; j < serial_size; j++)
	{
		output[i & eeprom_size_mask] = eeprom->serial[j], i++;
		output[i & eeprom_size_mask] = 0x00, i++;
	}
	
	// calculate checksum
	checksum = 0xAAAA;
	
	for (i = 0; i < eeprom->size/2-1; i++)
	{
		value = output[i*2];
		value += output[(i*2)+1] << 8;
	
		checksum = value^checksum;
		checksum = (checksum << 1) | (checksum >> 15);
	}
	
	output[eeprom->size-2] = checksum;
	output[eeprom->size-1] = checksum >> 8;
	return size_check;
}



int ftdi_66eeprom_decode(struct ftdi_eeprom *eeprom, unsigned char *buf, int size)
{
    unsigned char i, j;
    unsigned short checksum, eeprom_checksum, value;
    unsigned char manufacturer_size = 0, product_size = 0, serial_size = 0;
    //int size_check;
    int eeprom_size = 256;

    if (eeprom == NULL)
        return -1;
#if 0
    size_check = eeprom->size;
    size_check -= 28; // 28 are always in use (fixed)

    // Top half of a 256byte eeprom is used just for strings and checksum
    // it seems that the FTDI chip will not read these strings from the lower half
    // Each string starts with two bytes; offset and type (0x03 for string)
    // the checksum needs two bytes, so without the string data that 8 bytes from the top half
    if (eeprom->size>=256)size_check = 120;
    size_check -= manufacturer_size*2;
    size_check -= product_size*2;
    size_check -= serial_size*2;

    // eeprom size exceeded?
    if (size_check < 0)
        return (-1);
#endif

    // empty eeprom struct
    memset(eeprom, 0, sizeof(struct ftdi_eeprom));

    // Addr 00: High current IO
    eeprom->high_current = (buf[0x02] & HIGH_CURRENT_DRIVE);

    // Addr 02: Vendor ID
    eeprom->vendor_id = buf[0x02] + (buf[0x03] << 8);

    // Addr 04: Product ID
    eeprom->product_id = buf[0x04] + (buf[0x05] << 8);

    value = buf[0x06] + (buf[0x07]<<8);
    switch (value)
    {
        case 0x0900:
            eeprom->chip_type = TYPE_232H;
            break;
        case 0x0800:
            eeprom->chip_type = TYPE_4232H;
            break;
        case 0x0700:
            eeprom->chip_type = TYPE_2232H;
            break;
        case 0x0600:
            eeprom->chip_type = TYPE_R;
            break;
        case 0x0400:
            eeprom->chip_type = TYPE_BM;
            break;
        case 0x0200:
            eeprom->chip_type = TYPE_AM;
            break;
        default: // Unknown device
            eeprom->chip_type = 0;
            break;
    }

    // Addr 08: Config descriptor
    // Bit 7: always 1
    // Bit 6: 1 if this device is self powered, 0 if bus powered
    // Bit 5: 1 if this device uses remote wakeup
    // Bit 4: 1 if this device is battery powered
    j = buf[0x08];
    if (j&0x40) eeprom->self_powered = 1;
    if (j&0x20) eeprom->remote_wakeup = 1;

    // Addr 09: Max power consumption: max power = value * 2 mA
    eeprom->max_power = buf[0x09];

    // Addr 0A: Chip configuration
    // Bit 7: 0 - reserved
    // Bit 6: 0 - reserved
    // Bit 5: 0 - reserved
    // Bit 4: 1 - Change USB version
    // Bit 3: 1 - Use the serial number string
    // Bit 2: 1 - Enable suspend pull downs for lower power
    // Bit 1: 1 - Out EndPoint is Isochronous
    // Bit 0: 1 - In EndPoint is Isochronous
    //
    j = buf[0x0A];
    if (j&0x01) eeprom->in_is_isochronous = 1;
    if (j&0x02) eeprom->out_is_isochronous = 1;
    if (j&0x04) eeprom->suspend_pull_downs = 1;
    if (j&0x08) eeprom->use_serial = 1;
    if (j&0x10) eeprom->change_usb_version = 1;

    // Addr 0B: Invert data lines
    eeprom->invert = buf[0x0B];

    // Addr 0C: USB version low byte when 0x0A bit 4 is set
    // Addr 0D: USB version high byte when 0x0A bit 4 is set
    if (eeprom->change_usb_version == 1)
    {
        eeprom->usb_version = buf[0x0C] + (buf[0x0D] << 8);
    }

    // Addr 0E: Offset of the manufacturer string + 0x80, calculated later
    // Addr 0F: Length of manufacturer string
    manufacturer_size = buf[0x0F]/2;
    if (manufacturer_size > 0) eeprom->manufacturer = malloc(manufacturer_size);
    else eeprom->manufacturer = NULL;

    // Addr 10: Offset of the product string + 0x80, calculated later
    // Addr 11: Length of product string
    product_size = buf[0x11]/2;
    if (product_size > 0) eeprom->product = malloc(product_size);
    else eeprom->product = NULL;

    // Addr 12: Offset of the serial string + 0x80, calculated later
    // Addr 13: Length of serial string
    serial_size = buf[0x13]/2;
    if (serial_size > 0) eeprom->serial = malloc(serial_size);
    else eeprom->serial = NULL;

    // Addr 14: CBUS function: CBUS0, CBUS1
    // Addr 15: CBUS function: CBUS2, CBUS3
    // Addr 16: CBUS function: CBUS5
    if (eeprom->chip_type == TYPE_R) {
        eeprom->cbus_function[0] = buf[0x14] & 0x0f;
        eeprom->cbus_function[1] = (buf[0x14] >> 4) & 0x0f;
        eeprom->cbus_function[2] = buf[0x15] & 0x0f;
        eeprom->cbus_function[3] = (buf[0x15] >> 4) & 0x0f;
        eeprom->cbus_function[4] = buf[0x16] & 0x0f;
    } else {
        for (j=0; j<5; j++) eeprom->cbus_function[j] = 0;
    }

    // Decode manufacturer
    i = buf[0x0E] & 0xff; // offset
    for (j=0;j<manufacturer_size-1;j++)
    {
        eeprom->manufacturer[j] = buf[2*j+i+2];
    }
    eeprom->manufacturer[j] = '\0';

    // Decode product name
    i = buf[0x10] & 0xff; // offset
    for (j=0;j<product_size-1;j++)
    {
        eeprom->product[j] = buf[2*j+i+2];
    }
    eeprom->product[j] = '\0';

    // Decode serial
    i = buf[0x12] & 0xff; // offset
    for (j=0;j<serial_size-1;j++)
    {
        eeprom->serial[j] = buf[2*j+i+2];
    }
    eeprom->serial[j] = '\0';

    // verify checksum
    checksum = 0xAAAA;

    for (i = 0; i < eeprom_size/2-1; i++)
    {
        value = buf[i*2];
        value += buf[(i*2)+1] << 8;

        checksum = value^checksum;
        checksum = (checksum << 1) | (checksum >> 15);
    }

    eeprom_checksum = buf[eeprom_size-2] + (buf[eeprom_size-1] << 8);

    if (eeprom_checksum != checksum)
    {
        fprintf(stderr, "Checksum Error: %04x %04x\n", checksum, eeprom_checksum);
        return -1;
    }

    return 0;
}







void default232H_EEPROM(struct ftdi_eeprom *peeprom)
{
		peeprom->vendor_id=0x403;
		peeprom->product_id=0x6014;
		peeprom->self_powered=0;
		peeprom->remote_wakeup=0;
		peeprom->chip_type=0x6;/*232H*/
		peeprom->in_is_isochronous=0;
		peeprom->out_is_isochronous=0;
		peeprom->suspend_pull_downs=0;
		peeprom->use_serial=1;
		peeprom->change_usb_version=0;
		peeprom->usb_version=0;
		peeprom->max_power=50;/* value * 2 ma*/
		peeprom->high_current=0;
		peeprom->invert=0;
		peeprom->size=256;
		peeprom->cbus_function[0] = 0;
		peeprom->cbus_function[1] = 0;
		peeprom->cbus_function[2] = 0;
		peeprom->cbus_function[3] = 0;
		peeprom->cbus_function[4] = 0;
		peeprom->manufacturer = "FTDI";
		//peeprom->product = "UM232H";
		peeprom->product = "USB <-> Serial Converter";
	//	peeprom->serial = "1122334455667788";
}
void defaultEEPROM(struct ftdi_eeprom *peeprom)
{
		peeprom->vendor_id=0x403;
		peeprom->product_id=0x6010;
		peeprom->self_powered=0;
		peeprom->remote_wakeup=0;
		peeprom->chip_type=0x4;/*2232H*/
		peeprom->in_is_isochronous=0;
		peeprom->out_is_isochronous=0;
		peeprom->suspend_pull_downs=0;
		peeprom->use_serial=1;
		peeprom->change_usb_version=0;
		peeprom->usb_version=0;
		peeprom->max_power=50;/* value * 2 ma*/
		peeprom->high_current=0;
		peeprom->invert=0;
		peeprom->size=128;
		peeprom->cbus_function[0] = 0;
		peeprom->cbus_function[1] = 0;
		peeprom->cbus_function[2] = 0;
		peeprom->cbus_function[3] = 0;
		peeprom->cbus_function[4] = 0;
		peeprom->manufacturer = "FTDI";
		peeprom->product = "USB <-> Serial Converter";
	//	peeprom->serial = "1122334455667788";
}

int main(int argc, char* argv[])
{
	struct ftdi_context * ftdi_fp = NULL;
	struct ftdi_eeprom* peeprom = NULL;

	if(argc <= 1)
		goto help;

	ftdi_fp = ftdi_new();

	if(ftdi_init(ftdi_fp) != 0)
	{
		printf("init error\n");
		return -1;
	}

	if(ftdi_usb_open(ftdi_fp, VENDOR_ID, PRODUCT232H_ID) != 0)
	{
		printf("open fail\n");
		ftdi_free(ftdi_fp);
		return -1;
	}
	peeprom = malloc(sizeof(struct ftdi_eeprom));

	ftdi_eeprom_setsize(ftdi_fp, peeprom, EEPROM_SIZE);
	if(strcmp(argv[1], "read") == 0)
	{
		if(argc != 3)
			goto help;
		unsigned short value;
		int add = atoi(argv[2]);
		if((add < 0) || (add > 127))
			goto help;
		ftdi_read_eeprom_location(ftdi_fp,add,&value);
		printf("value[%d] =0x%x\n", add, value);
	}
	else if(strcmp(argv[1], "readall") == 0)
	{
		unsigned char* eeprom_buf = malloc(EEPROM_SIZE);
		//FILE *fp;
		//fp = fopen("test.bin", "w");
		memset(eeprom_buf, 0x00, EEPROM_SIZE);
		ftdi_read_eeprom(ftdi_fp, eeprom_buf);
		ftdi_66eeprom_decode(peeprom, eeprom_buf, EEPROM_SIZE);
		printf("vendor_id:0x%x\n", peeprom->vendor_id);
		printf("product_id:0x%x\n", peeprom->product_id);
		printf("self_powered:%d\n", peeprom->self_powered);
		printf("remote_wakeup:%d\n", peeprom->remote_wakeup);
		printf("chip_type:0x%x\n", peeprom->chip_type);
		printf("in_is_isochronous:%d\n", peeprom->in_is_isochronous);
		printf("out_is_isochronous:%d\n", peeprom->out_is_isochronous);
		printf("suspend_pull_downs:%d\n", peeprom->suspend_pull_downs);
		printf("use_serial:%d\n", peeprom->use_serial);
		printf("change_usb_version:%d\n", peeprom->change_usb_version);
		printf("usb_version:%d\n", peeprom->usb_version);
		printf("max_power:%d\n", peeprom->max_power);
		printf("high_current:%d\n", peeprom->high_current);
		printf("invert:%d\n", peeprom->invert);
		printf("size:%d\n", peeprom->size);
		printf("manufacturer:%s\n", peeprom->manufacturer);
		printf("product:%s\n", peeprom->product);
		printf("serial number:%s\n", peeprom->serial);
		printf("cbus_fun[0]:%d\n", peeprom->cbus_function[0]);
		printf("cbus_fun[1]:%d\n", peeprom->cbus_function[1]);
		printf("cbus_fun[2]:%d\n", peeprom->cbus_function[2]);
		printf("cbus_fun[3]:%d\n", peeprom->cbus_function[3]);
		printf("cbus_fun[4]:%d\n", peeprom->cbus_function[4]);
		//fwrite(eeprom_buf, 1, EEPROM_SIZE, fp);
		//fclose(fp);
		if(peeprom->manufacturer != NULL)
			free(peeprom->manufacturer);
		if(peeprom->product != NULL)
			free(peeprom->product);
		if(peeprom->serial != NULL)
			free(peeprom->serial);
		free(eeprom_buf);
	}
	else if(strcmp(argv[1], "erase") == 0)
	{
		printf("erase \n");
		ftdi_erase_eeprom(ftdi_fp);
	}
	else if(strcmp(argv[1], "write") == 0)
	{
		if(argc != 4)
			goto help;
		int add = atoi(argv[2]);
		if((add < 0) || (add > 127))
			goto help;
		unsigned short value = atoi(argv[3]);
		ftdi_write_eeprom_location(ftdi_fp,add,value);
		printf("value[%d] =0x%x\n", add, value);
	}
	else if(strcmp(argv[1], "write_serial") == 0)
	{
		int i = 0;
		if(argc != 3)
			goto help;
		char serialLen = strlen(argv[2]);
		if(serialLen != MAC_SIZE)
			goto help;
		for(i = 0; i < MAC_SIZE; i++)
		{
			char value = argv[2][i];
			if(((value >= 0x30) && (value <= 0x39)) || 
					((value >= 0x41) && (value <= 0x46)) || 
					((value >= 0x61) && (value <= 0x65)))
				continue;
			else
				goto help;
		}
		unsigned char* eeprom_buf = malloc(EEPROM_SIZE);
		memset(eeprom_buf, 0x00, EEPROM_SIZE);
		default232H_EEPROM(peeprom);
		peeprom->serial = malloc(MAC_SIZE+1);
		memset(peeprom->serial, 0x00, MAC_SIZE+1);
		memcpy(peeprom->serial, argv[2], MAC_SIZE);
		ftdi_232h_eeprom_build(peeprom, eeprom_buf); 
		ftdi_write_eeprom(ftdi_fp, eeprom_buf);
		free(peeprom->serial);
		free(eeprom_buf);
		printf("Write serial number Success.\n");
	}
	free(peeprom);
	ftdi_free(ftdi_fp);
	return 0;
help:
	printf("eeprom \n");
	printf("\treadall\n"); 
	printf("\t\t-Show all eeprom data\n");
	printf("\treal [add]\n");
	printf("\t\t-Need the address. Address is 0 ~ 127.\n");
	printf("\twrite [add] [data]\n");
	printf("\t\t-Need the address. Address is 0 ~ 127. Data is 0 ~ 255\n");
	printf("\terase\n");
	printf("\t\t-Clean eeprom data.\n");
	printf("\twrite_serial [number]\n");
	printf("\t\t-Write serial number 12 characters[0~9, a~f, A~F].\n");
	if(peeprom != NULL)
		free(peeprom);
	if(ftdi_fp != NULL)
		ftdi_free(ftdi_fp);
	return 0;
}
