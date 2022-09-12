#include <stdlib.h>
#include "commands.h"

int main() {
    int size;
    unsigned char **names = loadFile("teste.txt", &size);

    FILE *result = fopen("teste2.txt", "w");

    for (int i = 0; i < size; i++)
        fprintf(result, "%s", names[i]);

    fclose(result);

    return 0;
}