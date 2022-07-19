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
    char c;
    int sequence = 1;
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

            int send_len = sendMessage(mySocket, newMessage);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "rls")) {
            t_message *newMessage = buildMessage(newCommand, sequence, RLS);

            int send_len = sendMessage(mySocket, newMessage);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "get")) {
            t_message *newMessage = buildMessage(newCommand, sequence, GET);

            int send_len = sendMessage(mySocket, newMessage);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "put")) {
            t_message *newMessage = buildMessage(newCommand, sequence, PUT);

            int send_len = sendMessage(mySocket, newMessage);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "lmkdir")) {
            printf("Fazer lmkdir\n");
        }
        else if (!strcmp(newCommand->cmd, "rmkdir")) {
            t_message *newMessage = buildMessage(newCommand, sequence, RMKDIR);

            int send_len = sendMessage(mySocket, newMessage);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else {
            printf("Erro: comando desconhecido!\n");
        }
    }

} 