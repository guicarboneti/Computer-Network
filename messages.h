#ifndef __MESSAGE__
#define __MESSAGE__

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include<sys/types.h>

void printMessage(t_message *message);

t_message *buildMessage(t_command *command, int sequence, int cmdCode);

t_message *receiveMessage(int socket, int expected);

int sendMessage(int socket, t_message *message);

#endif