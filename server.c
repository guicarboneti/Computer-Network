
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
#include "messages.h"

int expected = 0;

int main() {
    int mySocket = ConexaoRawSocket("lo");
    if (mySocket < 0) {
        printf("Erro na conexão com socket\n");
        return 1;
    }

    chdir(HOMESERVER);

    printf("Escutando...\n");
    while (1) {
        
        t_message *receivedMessage = receiveMessage(mySocket, expected);

        expected++;
    }
}