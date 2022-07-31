#ifndef __UTILS__
#define __UTILS__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "types.h"

t_command *buildCommand(char *text);

double timestamp(void);

#endif