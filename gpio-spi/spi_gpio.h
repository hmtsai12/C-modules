#ifndef _SPI_GPIO_H_
#define _SPI_GPIO_H_
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "ralink_gpio.h"

#define GPIO_DEV "/dev/gpio"

#define GPIO_HIGH 1
#define GPIO_LOW 0
#define SPIGPIO54 54
#define SPIGPIO55 55
#define SPIGPIO56 56
#define SPIGPIO57 57

//#define SPIGPIO54 (1 << (54-40))
//#define SPIGPIO55 (1 << (55-40))
//#define SPIGPIO56 (1 << (56-40))
//#define SPIGPIO57 (1 << (57-40))
//#define SPIPINVALUE SPIGPIO54 | SPIGPIO55 | SPIGPIO56 | SPIGPIO57
enum
{
    gpio_in,
    gpio_out,
};
enum
{
    gpio2300,
    gpio3924,
    gpio7140,
    gpio72,
};

int switch_gpio(int gpioPin, int *value);
int gpio_open_dev(int mode);
int gpio_close_dev(int fd);
int gpio_set_dir(int fd, int r, int dir, unsigned long gpio);
int gpio_read_int(int fd, int r, int *value);
int gpio_write_int(int fd, int r, int value);
int gpio_set_int(int fd, int r, int value);
int gpio_clear_int(int fd, int r, int value);
void setGpioValue(int fd, uint16_t gpioPin, uint8_t active);
uint8_t getGpioValue(int fd, uint16_t gpioPin);
unsigned char spi_send_receive(int fd, unsigned char bData);
#endif /*_SPI_GPIO_H_*/
