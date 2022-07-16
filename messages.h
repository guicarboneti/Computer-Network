#ifndef __MESSAGE__
#define __MESSAGE__

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

t_message *buildMessage(t_command *command, int sequence, int cmdCode);

#endif