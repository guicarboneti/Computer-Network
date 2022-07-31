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

int main() {
    int mySocket = ConexaoRawSocket("lo");
    if (mySocket < 0) {
        printf("Erro na conexão com socket\n");
        return 1;
    }

    chdir(HOMESERVER);

    int sequence = 0;

    printf("Escutando...\n");
    while (1) {
        t_message *receivedMessage = receiveMessage(mySocket);
        if (receivedMessage != NULL) {
            if (receivedMessage->header.marker == STARTMARKER) {
                if (receivedMessage->header.sequence == sequence) {
                    printMessage(receivedMessage);
                    receivedMessage->header.type = OK;
                    int send_len = sendMessage(mySocket, receivedMessage);
                    if(send_len < 0){
                        printf("Erro ao enviar dados para socket.\n");
                    }
                    sequence++;
                }
                // if header's sequence is smaller than sequence, it means it's a doubly and can be ignored
                else if (receivedMessage->header.sequence > sequence) {
                    int send_len = sendNack(mySocket, receivedMessage);
                    if(send_len < 0){
                        printf("Erro ao enviar dados para socket.\n");
                    }
                }
            }
            else {
                int send_len = sendNack(mySocket, receivedMessage);
                if(send_len < 0){
                    printf("Erro ao enviar dados para socket.\n");
                }
            }
        }
    }

    return 0;
}