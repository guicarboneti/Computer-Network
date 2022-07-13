#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include<sys/types.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"

#include "types.h"

int main() {
    int mySocket = ConexaoRawSocket("lo");
    if (mySocket < 0) {
        printf("Erro na conexão com socket\n");
        return 1;
    }

    chdir(HOME);

    while (1) {
        unsigned char *buff = malloc(100 * sizeof(char));
        int buflen=recv(mySocket, buff, 100, 0);

        if (buflen<0) {
            printf("Erro ao receber dados do socket.\n");
        }
        else {
            printf("%s\n", buff);
        }

    }
}