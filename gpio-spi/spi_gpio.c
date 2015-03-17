#include "spi_gpio.h"

unsigned long spi_sck, spi_mosi, spi_miso, spi_csn;

//#if DEBUG_SPI == 1
	#define DEBUG_MSG(str)			fprintf(stderr, str)
	#define DEBUG_PRINTF(fmt, args...)	fprintf(stderr, "%s:%d: "fmt, __FUNCTION__, __LINE__, args)
	#define CHECK_NULL(a)			if(a==NULL){fprintf(stderr, "%s:%d ERROR: NULL POINTER AS ARGUMENT\n",__FUNCTION__, __LINE__);return LGW_SPI_ERROR;}
/*#else
	#define DEBUG_MSG(str)
	#define DEBUG_PRINTF(fmt, args...)
	#define CHECK_NULL(a)			if(a==NULL){return LGW_SPI_ERROR;}
#endif*/
#define DEBUG_ERROR(str)			fprintf(stderr, str);

int switch_gpio(int gpioPin, int *value)
{
	int gpiogroup;
	if(gpioPin <= 23)
	{
		gpiogroup = gpio2300;
		*value = 1 << (gpioPin - 0);
	}
	else if((gpioPin >=24) && (gpioPin <=39))
	{
		gpiogroup = gpio3924;
		*value = 1 << (gpioPin - 24);
	}
	else
	{
		gpiogroup = gpio7140;
		*value = 1 << (gpioPin - 40);
	}
	return gpiogroup;
}

int gpio_open_dev(int mode)
{
	int fd;
	int gpio_group = 0;
	int pin_value = 0;
	fd = open(GPIO_DEV, O_RDONLY);

	if (fd < 0)
	{
		perror(GPIO_DEV);
		return -1;
	}
	
	if(mode == 0)
	{
		DEBUG_MSG("OPEN SPI-GPIO 0.\n");
		DEBUG_MSG("GPIO 54,55,56,57.\n");
		spi_sck = SPIGPIO56;
		spi_miso = SPIGPIO57;
		spi_mosi = SPIGPIO54;
		spi_csn = SPIGPIO55;
		/*set direction*/
		gpio_group = switch_gpio(spi_miso, &pin_value);
		if(gpio_set_dir(fd, gpio_group, gpio_in, pin_value) != 0)	
		{
			DEBUG_MSG("SET direction in fail.\n");
			return -1;
		}
		gpio_group = switch_gpio(spi_mosi, &pin_value);
		if(gpio_set_dir(fd, gpio_group, gpio_out, pin_value)!= 0)	
		{
			DEBUG_MSG("SET direction out fail.\n");
			return -1;
		}
		gpio_group = switch_gpio(spi_csn, &pin_value);
		if(gpio_set_dir(fd, gpio_group, gpio_out, pin_value)!= 0)	
		{
			DEBUG_MSG("SET direction out fail.\n");
			return -1;
		}
		gpio_group = switch_gpio(spi_sck, &pin_value);
		if(gpio_set_dir(fd, gpio_group, gpio_out, pin_value)!= 0)	
		{
			DEBUG_MSG("SET direction out fail.\n");
			return -1;
		}
		setGpioValue(fd, spi_sck, GPIO_LOW);
		setGpioValue(fd, spi_csn, GPIO_LOW);
		setGpioValue(fd, spi_mosi, GPIO_LOW);
	}
	else
	{
		DEBUG_MSG("Please set other SPI-GPIO configure.\n");
	}
	return fd;
}

int gpio_close_dev(int fd)
{
	return close(fd);
}

int gpio_set_dir(int fd, int r, int dir, unsigned long gpio)
{
    int req;

   // fd = open(GPIO_DEV, O_RDONLY);
    if (fd < 0)
    {
        perror(GPIO_DEV);
        return -1;
    }
    if (dir == gpio_in)
    {
        if (r == gpio72)
            req = RALINK_GPIO72_SET_DIR_IN;
        else if (r == gpio7140)
            req = RALINK_GPIO7140_SET_DIR_IN;
        else if (r == gpio3924)
            req = RALINK_GPIO3924_SET_DIR_IN;
        else
            req = RALINK_GPIO_SET_DIR_IN;
    }
    else
    {
        if (r == gpio72)
            req = RALINK_GPIO72_SET_DIR_OUT;
        else if (r == gpio7140)
            req = RALINK_GPIO7140_SET_DIR_OUT;
        else if (r == gpio3924)
            req = RALINK_GPIO3924_SET_DIR_OUT;
        else
            req = RALINK_GPIO_SET_DIR_OUT;
    }
    //if (ioctl(fd, req, 0xffffffff) < 0)
    if (ioctl(fd, req, gpio) < 0)
    {
        perror("ioctl");
        //close(fd);
        return -1;
    }
    //close(fd);
    return 0;
}

int gpio_read_int(int fd, int r, int *value)
{
    int req;

    *value = 0;
    //fd = open(GPIO_DEV, O_RDONLY);
    if (fd < 0)
    {
        perror(GPIO_DEV);
        return -1;
    }

    if (r == gpio72)
        req = RALINK_GPIO72_READ;
    else if (r == gpio7140)
        req = RALINK_GPIO7140_READ;
    else if (r == gpio3924)
        req = RALINK_GPIO3924_READ;
    else
        req = RALINK_GPIO_READ;
    if (ioctl(fd, req, value) < 0)
    {
        perror("ioctl");
        //close(fd);
        return -1;
    }
    //close(fd);
    DEBUG_PRINTF("READ gpio DATA[%d]=0x%x\n", r, *value); 
    return 0;
}

int gpio_write_int(int fd, int r, int value)
{
    int req;

    //fd = open(GPIO_DEV, O_RDONLY);
    if (fd < 0)
    {
        perror(GPIO_DEV);
        return -1;
    }
    if (r == gpio72)
        req = RALINK_GPIO72_WRITE;
    else if (r == gpio7140)
        req = RALINK_GPIO7140_WRITE;
    else if (r == gpio3924)
        req = RALINK_GPIO3924_WRITE;
    else
        req = RALINK_GPIO_WRITE;
    if (ioctl(fd, req, value) < 0)
    {
        perror("ioctl");
        //close(fd);
        return -1;
    }
    //close(fd);
    return 0;
}
int gpio_set_int(int fd, int r, int value)
{
    int req;

    //fd = open(GPIO_DEV, O_RDONLY);
    if (fd < 0)
    {
        perror(GPIO_DEV);
        return -1;
    }
    if (r == gpio72)
        req = RALINK_GPIO72_SET;
    else if (r == gpio7140)
        req = RALINK_GPIO7140_SET;
    else if (r == gpio3924)
        req = RALINK_GPIO3924_SET;
    else
        req = RALINK_GPIO_SET;
    if (ioctl(fd, req, value) < 0)
    {
        perror("ioctl");
        //close(fd);
        return -1;
    }
    //close(fd);
    return 0;
}
int gpio_clear_int(int fd, int r, int value)
{
    int req;

    //fd = open(GPIO_DEV, O_RDONLY);
    if (fd < 0)
    {
        perror(GPIO_DEV);
        return -1;
    }
    if (r == gpio72)
        req = RALINK_GPIO72_CLEAR;
    else if (r == gpio7140)
        req = RALINK_GPIO7140_CLEAR;
    else if (r == gpio3924)
        req = RALINK_GPIO3924_CLEAR;
    else
        req = RALINK_GPIO_CLEAR;
    if (ioctl(fd, req, value) < 0)
    {
        perror("ioctl");
        //close(fd);
        return -1;
    }
    //close(fd);
    return 0;
}

void setGpioValue(int fd, uint16_t gpioPin, uint8_t active)
{
	int gpio_group = 0;
	int pin_value = 0;

	DEBUG_PRINTF("gpio=%d, active=%d\n", gpioPin, active);

	gpio_group = switch_gpio(gpioPin, &pin_value);

	if(active == GPIO_HIGH)
	{
		if(gpio_set_int(fd, gpio_group, pin_value) == -1)DEBUG_ERROR("ERROR: set gpio data Fail\n");
	}
	else/*GPIO_LOW*/
	{
		if(gpio_clear_int(fd, gpio_group, pin_value) == -1)DEBUG_ERROR("ERROR: clear gpio data Fail\n");
	}
}

uint8_t getGpioValue(int fd, uint16_t gpioPin)
{
	int gpio_group = 0;
	int pin_value = 0;
	int data = 0;

	gpio_group = switch_gpio(gpioPin, &pin_value);
	
	if(gpio_read_int(fd, gpio_group, &data) == -1)DEBUG_ERROR("ERROR: read gpio data Fail\n");

	pin_value &= data;

	if(pin_value == 0)
		return GPIO_LOW;
	else
		return GPIO_HIGH;
}

unsigned char spi_send_receive(int fd, unsigned char bData)
{
	unsigned char InByte, bit;
	InByte = 0x00;
	DEBUG_PRINTF("send data =%d\n", bData);
	//setGpioValue(fd, spi_csn, GPIO_HIGH);
//	usleep(50);
	for(bit = 0x80; bit; bit >>= 1)
	{
		setGpioValue(fd, spi_mosi, bData & bit ? GPIO_HIGH : GPIO_LOW);
		usleep(50);
		setGpioValue(fd, spi_sck, GPIO_HIGH);
		usleep(50);
		if(getGpioValue(fd, spi_miso) == GPIO_HIGH) InByte |= bit;
		usleep(50);
		setGpioValue(fd, spi_sck, GPIO_LOW);
	}

		usleep(50);
	setGpioValue(fd, spi_sck, GPIO_LOW);
		usleep(50);
	setGpioValue(fd, spi_mosi, GPIO_LOW);
	usleep(50);
//	setGpioValue(fd, spi_csn, GPIO_LOW);
//		usleep(50);
	return InByte;	
}
