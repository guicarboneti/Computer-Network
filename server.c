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
#include "commands.h"

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
            char calc_parity = calculateParity(receivedMessage);
            if ((receivedMessage->header.marker == STARTMARKER) && (compareParity(calc_parity, receivedMessage->parity))) {
                if (receivedMessage->header.sequence == sequence) {
                    if (receivedMessage->header.type == RCD) {
                        char res = lcd(receivedMessage->data);
                        int send_len;

                        if (res == OK)
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, OK, res);
                        else
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ERROR, res);
                        if (send_len < 0) {
                            printf("Erro ao enviar dados para socket.\n");
                        }
                    }
                    else if (receivedMessage->header.type == RMKDIR) {
                        char res = lmkdir(receivedMessage->data);
                        int send_len;

                        if (res == OK)
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, OK, res);
                        else
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ERROR, res);
                        if (send_len < 0) {
                            printf("Erro ao enviar dados para socket.\n");
                        }
                    }
                    else if (receivedMessage->header.type == RLS) {
                        char *delim = ",";
                        char *ptr = strtok(receivedMessage->data, delim);

                        char args[3] = "";
                        while (ptr != NULL) {
                            if (strcmp(ptr, "a") == 0)
                                strcpy(args, "-a");
                            else if (strcmp(ptr, "l") == 0)
                                strcpy(args, "-l");

                            ptr = strtok(NULL, delim);
                        }

                        printf("%s\n", args);

                        char **names;
                        int size;
                        char res = lls(args, &size, &names);

                        int send_len;
                        if (res == OK) {
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ACK, res);
                            // create function to send names array to client
                            free(names);
                        }
                        else
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ERROR, res);

                        if (send_len < 0) {
                            printf("Erro ao enviar dados para socket.\n");
                        }
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