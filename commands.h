#ifndef __COMMAND__
#define __COMMAND__

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include<sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


// mudar para strings de argumentos
char lcd(t_command *command);

char lls(char *arg, int *size, char ***names);

#endif