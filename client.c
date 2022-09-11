#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"

#include "types.h"
#include "utils.h"
#include "messages.h"
#include "commands.h"

int main() {
    int mySocket = ConexaoRawSocket("lo");

    if (mySocket < 0) {
        printf("Erro na conexão com socket\n");
        return 1;
    }

    chdir(HOMECLIENT);

    char command[100];
    char c, errorCode;
    int sequence = 0;
    printf("Enviando...\n");


    while (1) {
        scanf("%[^\n]%c", command, &c);
        t_command *newCommand = buildCommand(command);

        if (!strcmp(newCommand->cmd, "lcd")) {
            if (newCommand->numArgs > 0) {
                char res = lcd(newCommand->args[0]);
                if (res == OK)
                    printf("Entrou no diretório %s\n", newCommand->args[0]);
                else {
                    switch (res) {
                    case DIRECTORYNOTEXISTANT:
                        printf("Erro: diretório não existe!\n");
                        break;
                    
                    case WITHOUTPERMISSION:
                        printf("Erro: sem permissão!\n");
                        break;

                    default:
                        printf("Erro!\n");
                        break;
                    }
                }
            }
        }

        else if (!strcmp(newCommand->cmd, "lls")) {
            char **names;
            int size;

            char args[3] = "";
            for (int i = 0; i < newCommand->numArgs; i++) {
                if (strcmp(newCommand->args[i], "-a") == 0)
                    strcpy(args, "-a");
                else if (strcmp(newCommand->args[i], "-l") == 0)
                    strcpy(args, "-l");
            }
            char res = lls(args, &size, &names);

            if (res == OK)
                for (int i = 0; i < size; i++)
                    printf("%s\n", names[i]);

            else {
                switch(res) {
                    case DIRECTORYNOTEXISTANT:
                        printf("Erro: diretório não existe!\n");
                        break;
                    default:
                        printf("Erro!");
                        break;
                }
            }

            free(names);
        }

        else if (!strcmp(newCommand->cmd, "rcd")) {
            if (newCommand->numArgs > 0) {
                t_message *newMessage = buildMessage(newCommand, sequence, RCD);

                char response = NACK;
                while (response == NACK) {
                    int send_len = sendMessage(mySocket, newMessage);
                    if(send_len < 0){
                        printf("Erro ao enviar dados para socket.\n");
                    }
                    response = awaitServerResponse(mySocket, &errorCode, sequence);
                }
                if (response == ERROR) {
                    switch (errorCode) {
                    case DIRECTORYNOTEXISTANT:
                        printf("Erro: diretório não existe!\n");
                        break;
                    
                    case WITHOUTPERMISSION:
                        printf("Erro: sem permissão!\n");
                        break;

                    default:
                        printf("Erro!\n");
                        break;
                    }
                    sequence = (sequence+1) % 16;
                }
                else if (response == OK) {
                    printf("Entrou no diretório %s\n", newCommand->args[0]);
                    sequence = (sequence+1) % 16;
                }
            }
        }

        else if (!strcmp(newCommand->cmd, "rls")) {
            t_message *newMessage = buildMessage(newCommand, sequence, RLS);

            char response = NACK;
            while (response == NACK) {
                int send_len = sendMessage(mySocket, newMessage);
                if(send_len < 0){
                    printf("Erro ao enviar dados para socket.\n");
                }
                response = awaitServerResponse(mySocket, &errorCode, sequence);
            }

            if (response == ERROR) {
                switch (errorCode) {
                case DIRECTORYNOTEXISTANT:
                    printf("Erro: diretório não existe!\n");
                    break;
                default:
                    printf("Erro!");
                    break;
                }
                sequence = (sequence+1) % 16;
            }
            else if (response == ACK) {
                printf("Receber diretórios!\n\n");

                char calc_parity;
                int send_len, i;
                t_message *receivedMessage = receiveMessage(mySocket);
                if (receivedMessage != NULL) {
                    while (receivedMessage->header.type != END) {
                        calc_parity = calculateParity(receivedMessage);
                        if ((receivedMessage->header.marker == STARTMARKER) && (compareParity(calc_parity, receivedMessage->parity))) {
                            if (receivedMessage->header.sequence == sequence) {
                                if (receivedMessage && receivedMessage->header.type == PRINT) {
                                    for (i=0; i<receivedMessage->header.size; i++)
                                        printf("%c", receivedMessage->data[i]);
                                    printf("\n");
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
                        printf("\nDiretórios recebidos!\n");
                }
            }
        }
        else if (!strcmp(newCommand->cmd, "get")) {
            t_message *newMessage = buildMessage(newCommand, sequence, GET);

            char response = NACK;
            while (response == NACK) {
                int send_len = sendMessage(mySocket, newMessage);
                if(send_len < 0){
                    printf("Erro ao enviar dados para socket.\n");
                }
                response = awaitServerResponse(mySocket, &errorCode, sequence);
            }
            if (response == ERROR) {
                printf("Erro!");
                // trata erro
                sequence = (sequence+1) % 16;
            }
            else if ((response == ACK) || (response == OK)) {
                printf("Ok!\n");
                sequence = (sequence+1) % 16;
            }
        }
        else if (!strcmp(newCommand->cmd, "put")) {
            t_message *newMessage = buildMessage(newCommand, sequence, PUT);

            int send_len = sendMessage(mySocket, newMessage);
            printf("Enviando arquivo...\n");
            if (put(mySocket, newCommand->args[0]) == OK)
                printf("Arquivo enviado!\n");
            else printf("Erro ao enviar arquivo para o servidor\n");
            sequence = (sequence+1) % 16;
        }
        else if (!strcmp(newCommand->cmd, "lmkdir")) {
            if (newCommand->numArgs > 0) {
                char res = lmkdir(newCommand->args[0]);
                if (res == OK)
                    printf("Diretório foi criado com sucesso.\n");
                else {
                    switch (res) {
                    case WITHOUTPERMISSION:
                        printf("Erro: sem permissão!\n");
                        break;

                    case DIRECTORYALREADYEXISTS:
                        printf("Erro: diretório já existe!\n");
                        break;
                    
                    default:
                        printf("Erro!\n");
                        break;
                    }
                }
            }
        }
        else if (!strcmp(newCommand->cmd, "rmkdir")) {
            t_message *newMessage = buildMessage(newCommand, sequence, RMKDIR);

            char response = NACK;
            while (response == NACK) {
                int send_len = sendMessage(mySocket, newMessage);
                if(send_len < 0){
                    printf("Erro ao enviar dados para socket.\n");
                }
                response = awaitServerResponse(mySocket, &errorCode, sequence);
            }
            if (response == ERROR) {
                switch (errorCode) {
                case WITHOUTPERMISSION:
                    printf("Erro: sem permissão!\n");
                    break;

                case DIRECTORYALREADYEXISTS:
                    printf("Erro: diretório já existe!\n");
                    break;
                
                default:
                    printf("Erro!\n");
                    break;
                }
                sequence = (sequence+1) % 16;
            }
            else if (response == OK) {
                printf("Diretório foi criado com sucesso.\n");
                sequence = (sequence+1) % 16;
            }
        }
        else {
            printf("Erro: comando desconhecido!\n");
        }
    }
    
    return 0;
}
