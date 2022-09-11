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

                        char **names = (char **)calloc(63, sizeof(char*));
                        int size;
                        char res = lls(args, &size, &names);

                        int send_len;
                        if (res == OK) {
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ACK, res);
                            rls(mySocket, size, names);
                            free(names);
                        }
                        else
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ERROR, res);

                        if (send_len < 0) {
                            printf("Erro ao enviar dados para socket.\n");
                        }
                    }
                    else if (receivedMessage->header.type == PUT) {

                        printf("Recebendo arquivo...\n");

                        char calc_parity;
                        int send_len, i;
                        char *filename;
                        t_message *receivedMessage = receiveMessage(mySocket);
                        if (receivedMessage != NULL) {
                            if (receivedMessage->header.type == PUT) {
                                filename = receivedMessage->data;
                                FILE *fd = fopen(filename, "w");    // discards content if file exists
                                fclose (fd);
                            }
                            while (receivedMessage->header.type != END) {
                                calc_parity = calculateParity(receivedMessage);
                                if ((receivedMessage->header.marker == STARTMARKER) && (compareParity(calc_parity, receivedMessage->parity))) {
                                    if (receivedMessage->header.sequence == sequence) {
                                        if (receivedMessage->header.type == FILEDESC) {
                                            FILE *fp;
                                            fp = fopen (filename, "a+");

                                            int ret = fwrite (receivedMessage->data, sizeof(unsigned char), receivedMessage->header.size, fp);

                                            int send_len;
                                            if (ret)
                                                send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ACK, ACK);
                                            else
                                                send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, NACK, NACK);
                                            if (send_len < 0) {
                                                printf("Erro ao enviar dados para socket.\n");
                                            }

                                            fclose(fp);
                                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ACK, ACK);
                                            sequence = (sequence+1) % 16;
                                        } else {
                                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, NACK, NACK);
                                        }
                                        if (send_len < 0)
                                            printf("Erro ao enviar dados para socket.\n");
                                    }
                                    // if header's sequence is smaller than sequence, it means it's a doubly and can be ignored
                                    else if (receivedMessage->header.sequence > sequence) {
                                        send_len = sendNack(mySocket, receivedMessage);
                                        if(send_len < 0){
                                            printf("Erro ao enviar dados para socket.\n");
                                        }
                                    }
                                }
                                else {
                                    send_len = sendNack(mySocket, receivedMessage);
                                    if(send_len < 0) {
                                        printf("Erro ao enviar dados para socket.\n");
                                    }
                                }
                                receivedMessage = receiveMessage(mySocket);
                            }
                            if (receivedMessage && receivedMessage->header.type == END)
                                printf("Arquivo recebido!\n");
                        }

                    }
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
            sequence = (sequence+1) % 16;
        }
    }

    return 0;
}