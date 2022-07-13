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

int main() {
    int mySocket = ConexaoRawSocket("lo");

    if (mySocket < 0) {
        printf("Erro na conexão com socket\n");
        return 1;
    }

    chdir(HOMECLIENT);

    char command[100];
    while (1) {
        scanf("%s", command);

        t_message * newMessage = malloc(sizeof(t_message));

        if (!strcmp(command, "lcd")) {
            printf("Fazer lcd\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(command, "lls")) {
            printf("Fazer lls\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(command, "rcd")) {
            printf("Fazer rcd\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(command, "rls")) {
            printf("Fazer rls\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(command, "get")) {
            printf("Fazer get\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(command, "put")) {
            printf("Fazer put\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(command, "lmkdir")) {
            printf("Fazer lmkdir\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else if (!strcmp(command, "rmkdir")) {
            printf("Fazer rmkdir\n");
            int send_len = send(mySocket, command, sizeof(command), 0);
            if(send_len < 0){
                printf("Erro ao enviar dados para socket.\n");
            }
        }
        else {
            printf("Erro: comando desconhecido!\n");
        }
    }
    
}