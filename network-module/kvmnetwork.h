#ifndef _KVMNETWORK_H_
#define _KVMNETWORK_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#define BUFSIZE 1024

int openUDPConnectSocket(int port);
char processUDPRequest(int s);
void closeUDPConnectSocket(int fd);
int openTCPConnectSocket(int port);
char processTCPRequest(int s);
void closeTCPConnectSocket(int fd);

#endif /*_KVMNETWORK_H_*/
