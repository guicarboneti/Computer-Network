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


char lcd(char *dir);

char lls(char *arg, int *size, char ***names);

char lmkdir(char *name);

#endif