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

#define CMDREADSIZE 200

int main() {
    int mySocket = ConexaoRawSocket("lo");

    if (mySocket < 0) {
        printf("Erro na conexão com socket\n");
        return 1;
    }

    chdir(HOMECLIENT);

    char command[CMDREADSIZE];
    while (1) {
        scanf("%[^\n]", command);

        t_message *newMessage = malloc(sizeof(t_message));
        t_command *newCommand = malloc(sizeof(t_command));

        newCommand = getCommand(command);

        /*
        if (!strcmp(newCommand->cmd, "lcd")) {
            printf("Fazer lcd\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "lls")) {
            printf("Fazer lls\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "rcd")) {
            printf("Fazer rcd\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "rls")) {
            printf("Fazer rls\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "get")) {
            printf("Fazer get\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "put")) {
            printf("Fazer put\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "lmkdir")) {
            printf("Fazer lmkdir\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(newCommand->cmd, "rmkdir")) {
            printf("Fazer rmkdir\n");
            int send_len = send(mySocket, newCommand->cmd, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else {
            printf("Erro: comando desconhecido!\n");
        }
        */
    }
    
}