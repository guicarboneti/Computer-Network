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

<<<<<<< HEAD
char loadFile(char *fileName, long *size, unsigned char **fileData, char *errorCode);
=======
unsigned char **loadFile(char *filename, int *blocks, char *res);
>>>>>>> d61be8a928f269571416a3c88492fef63ca1b9b9

#endif