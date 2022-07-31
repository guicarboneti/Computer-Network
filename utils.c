#include "utils.h"

t_command *buildCommand(char *text) {
    t_command *newCommand = malloc(sizeof(t_command));
    newCommand->numArgs = 0;
    newCommand->args = (char **)calloc(10, sizeof(unsigned char*));;
    char *delim = " ";
    char *ptr = strtok(text, delim);
    newCommand->cmd = ptr;

    while (ptr != NULL) {
		newCommand->args[newCommand->numArgs] = (unsigned char *) calloc(strlen(ptr), sizeof(unsigned char));
		ptr = strtok(NULL, delim);
        newCommand->args[newCommand->numArgs] = ptr;
        newCommand->numArgs++;
	}

    newCommand->numArgs--;

    return newCommand;
}

double timestamp(void) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return((double)(tp.tv_sec*1000.0 + tp.tv_usec/1000.0));
}