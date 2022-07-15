#include "utils.h"

t_command *getCommand(char *text) {
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