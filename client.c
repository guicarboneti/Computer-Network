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
            printf("Fazer lcd\n");
        }

        else if (!strcmp(newCommand->cmd, "lls")) {
            printf("Fazer lls\n");
        }

        else if (!strcmp(newCommand->cmd, "rcd")) {
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
                printf("Erro!");
                // trata erro
                sequence++;
            }
            else if ((response == ACK) || (response == OK)) {
                printf("Ok!\n");
                sequence++;
            }
            else if (response == TIMEOUT)
                printf("Timeout! Tente novamente\n");
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
                printf("Erro!");
                // trata erro
                sequence++;
            }
            else if ((response == ACK) || (response == OK)) {
                printf("Ok!\n");
                sequence++;
            }
            else if (response == TIMEOUT)
                printf("Timeout! Tente novamente\n");
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
                sequence++;
            }
            else if ((response == ACK) || (response == OK)) {
                printf("Ok!\n");
                sequence++;
            }
            else if (response == TIMEOUT)
                printf("Timeout! Tente novamente\n");
        }
        else if (!strcmp(newCommand->cmd, "put")) {
            t_message *newMessage = buildMessage(newCommand, sequence, PUT);

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
                sequence++;
            }
            else if ((response == ACK) || (response == OK)) {
                printf("Ok!\n");
                sequence++;
            }
            else if (response == TIMEOUT)
                printf("Timeout! Tente novamente\n");
        }
        else if (!strcmp(newCommand->cmd, "lmkdir")) {
            printf("Fazer lmkdir\n");
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
                printf("Erro!");
                // trata erro
                sequence++;
            }
            else if ((response == ACK) || (response == OK)) {
                printf("Ok!\n");
                sequence++;
            }
            else if (response == TIMEOUT)
                printf("Timeout! Tente novamente\n");
        }
        else {
            printf("Erro: comando desconhecido!\n");
        }
    }
    
    return 0;
}

