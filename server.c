#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include<sys/types.h>
#include <unistd.h>
#include <errno.h>

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
                        sequence = (sequence + 1) % 16;
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
                        sequence = (sequence + 1) % 16;
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
                        
                        char **names = (char **)calloc(63, sizeof(char*));
                        int size;
                        char res = lls(args, &size, &names);

                        int send_len;
                        if (res == OK) {
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ACK, res);
                            char c;
                            int seq_start_window;
                            for (int i = 0; i < size; i ++) {
                                if (i%4 == 0)
                                    seq_start_window = sequence;
                                sequence = (sequence + 1) % 16;
                                t_message *dirMsg = malloc(sizeof(t_message));
                                dirMsg->header.marker = STARTMARKER;
                                dirMsg->header.sequence = sequence;
                                dirMsg->header.size = strlen(names[i]);
                                dirMsg->header.type = PRINT;

                                dirMsg->data = malloc(dirMsg->header.size);
                                int j;
                                for (j = 0; j < strlen(names[i]); j++)
                                    dirMsg->data[j] = names[i][j];
                                dirMsg->data[j] = '\0';

                                dirMsg->parity = calculateParity(dirMsg);
                                send_len = sendMessage(mySocket, dirMsg);
                                if(send_len < 0){
                                    printf("Erro ao enviar dados para socket.\n");
                                }
                                char res = awaitServerResponse(mySocket, &c, sequence);
                                if (res == NACK) {
                                    // sequence = seq_start_window;
                                    i -= i % 4;
                                }
                            }
                            sequence = (sequence + 1) % 16;
                            send_len = sendOkErrorResponse(mySocket, sequence, END, END);
                            if(send_len < 0){
                                printf("Erro ao enviar dados para socket.\n");
                            }
                        }
                        else
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ERROR, res);

                        if (send_len < 0) {
                            printf("Erro ao enviar dados para socket.\n");
                        }
                        free(names);
                        sequence = (sequence + 1) % 16;
                    }
                    else if (receivedMessage->header.type == GET) {
                        unsigned char *fileData;
                        long size;
                        char errorCode;

                        char res = loadFile(receivedMessage->data, &size, &fileData, &errorCode);
                        int send_len;

                        if (res == ERROR) {
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ERROR, res);
                            if (send_len < 0) {
                                printf("Erro ao enviar dados para socket.\n");
                            }
                        }                        
                        else {
                            send_len = sendOkErrorResponse(mySocket, receivedMessage->header.sequence, ACK, res);
                            if (send_len < 0) {
                                printf("Erro ao enviar dados para socket.\n");
                            }
                            char c;
                            long indexFile = 0;
                            int i, idx_window_start, seq_window_start;
                            for (i=0; indexFile < size; i++) {
                                if (i%4 == 0) {
                                    idx_window_start = indexFile;
                                    // seq_window_start = sequence;
                                }
                                sequence = (sequence + 1) % 16;
                                t_message *dirMsg = malloc(sizeof(t_message));
                                dirMsg->header.marker = STARTMARKER;
                                dirMsg->header.sequence = sequence;
                                dirMsg->header.type = FILEDESC;

                                int j = 0;
                                char messageData[63];
                                while ((indexFile < size) && (j < 63)) {
                                    messageData[j] = fileData[indexFile];
                                    j++;
                                    indexFile++;
                                }

                                dirMsg->header.size = j * sizeof(unsigned char);
                                dirMsg->data = malloc(j * sizeof(unsigned char));
                                
                                for (int k = 0; k < j; k++) {
                                    dirMsg->data[k] = messageData[k];
                                }

                                dirMsg->parity = calculateParity(dirMsg);
                                send_len = sendMessage(mySocket, dirMsg);
                                if(send_len < 0){
                                    printf("Erro ao enviar dados para socket.\n");
                                }
                                char res = awaitServerResponse(mySocket, &c, sequence);
                                if (res == NACK) {
                                    // sequence = seq_window_start;
                                    indexFile = idx_window_start;
                                }
                            }
                            sequence = (sequence + 1) % 16;
                            send_len = sendOkErrorResponse(mySocket, sequence, END, END);
                            if(send_len < 0){
                                printf("Erro ao enviar dados para socket.\n");
                            }
                        }

                        sequence = (sequence + 1) % 16;
                    }
                    else if (receivedMessage->header.type == PUT) {
                        FILE *f = fopen(receivedMessage->data, "wb");
                        int send_len;
                        if (!f) {
                            switch (errno) {
                            case ENOMEM:
                                send_len = sendOkErrorResponse(mySocket, sequence, ERROR, NOSPACE);
                                if(send_len < 0){
                                    printf("Erro ao enviar dados para socket.\n");
                                }
                                break;

                            case ENOSPC:
                                send_len = sendOkErrorResponse(mySocket, sequence, ERROR, NOSPACE);
                                if(send_len < 0){
                                    printf("Erro ao enviar dados para socket.\n");
                                }
                                break;

                            case EACCES:
                                send_len = sendOkErrorResponse(mySocket, sequence, ERROR, WITHOUTPERMISSION);
                                if(send_len < 0){
                                    printf("Erro ao enviar dados para socket.\n");
                                }
                                break;
                            
                            default:
                                send_len = sendOkErrorResponse(mySocket, sequence, ERROR, OTHER);
                                if(send_len < 0){
                                    printf("Erro ao enviar dados para socket.\n");
                                }
                                break;
                            }
                        }
                        else {
                            sendOkErrorResponse(mySocket, sequence, OK, OK);
                            t_message *resMsg;
                            char serverMessageType = FILEDESC;
                            sequence = (sequence + 1) % 16;
                            while (serverMessageType != END) {
                                resMsg = receiveMessage(mySocket);
                                if (resMsg != NULL) {
                                    serverMessageType = resMsg->header.type;
                                    if (serverMessageType == FILEDESC) {
                                        char calc_parity = calculateParity(resMsg);
                                        if ((resMsg->header.sequence == sequence) && (compareParity(calc_parity, resMsg->parity))) {
                                            for (int i = 0; i < resMsg->header.size; i++) {
                                                fprintf(f, "%c", resMsg->data[i]);
                                            }
                                            sendOkErrorResponse(mySocket, sequence, ACK, ACK);
                                            sequence = (sequence + 1) % 16;
                                        }
                                        else if ((resMsg->header.sequence > sequence) || (!compareParity(calc_parity, resMsg->parity))) {
                                            sendOkErrorResponse(mySocket, sequence, NACK, NACK);
                                        }
                                    }
                                }
                            }
                            fclose(f);
                            printf("Arquivo recebido com sucesso.\n");
                            sequence = (sequence + 1) % 16;
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
        }
    }

    return 0;
}