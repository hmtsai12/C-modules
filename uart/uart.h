#ifndef _UART_H_
#define _UART_H_

#define BAUDRATE B115200
#define MODEMDEVICE	"/dev/ttyUSB0"

char uartConnect(char* port);
void uartBaudrate(speed_t speed);
char *uartCommand(char *command);
void uartDisconnect(void);
#endif /*_UART_H_*/
