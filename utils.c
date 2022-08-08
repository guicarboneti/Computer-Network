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

void charToBinary(int *bits, char c) {
    int i;
    for(i=0; i<8; i++)
        bits[i] = (c & (1 << 7-i)) ? '1' : '0';
}

int binaryToDecimal(int *n) {
    int i;
    int decimal_value = 0;
    int base = 1;

    for (i=7; i>=0; i--) {
        if (n[i] == 1)
            decimal_value += base;
        base = base * 2;
    }

    return decimal_value;
}