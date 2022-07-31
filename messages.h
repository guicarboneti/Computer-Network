#ifndef __MESSAGE__
#define __MESSAGE__

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include<sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

void printMessage(t_message *message);

t_message *buildMessage(t_command *command, int sequence, int cmdCode);

t_message *receiveMessage(int socket);

int sendMessage(int socket, t_message *message);

int sendNack(int socket, t_message *message);

char awaitServerResponse(int socket, char *errorCode, int sequence);

#endif