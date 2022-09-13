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

    char dir[60];
    while (1) {
        getcwd(dir,60);
        printf("%s> ",dir);
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
                    sequence = (sequence + 1) % 16;
                }
                else if (response == OK) {
                    printf("Entrou no diretório %s\n", newCommand->args[0]);
                    sequence = (sequence + 1) % 16;
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
            sequence = (sequence + 1) % 16;
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
            }
            else if (response == ACK) {
                t_message *resMsg;
                char serverMessageType = PRINT;
                while (serverMessageType != END) {
                    resMsg = receiveMessage(mySocket);
                    if (resMsg != NULL) {
                        serverMessageType = resMsg->header.type;
                        if ((serverMessageType == PRINT)) {
                            char calc_parity = calculateParity(resMsg);
                            if ((resMsg->header.sequence == sequence) && (compareParity(calc_parity, resMsg->parity))) {
                                printf("%s\n", resMsg->data);
                                sendOkErrorResponse(mySocket, sequence, ACK, ACK);
                                sequence = (sequence + 1) % 16;
                            }
                            else if ((resMsg->header.sequence > sequence) || (!compareParity(calc_parity, resMsg->parity))) {
                                sendOkErrorResponse(mySocket, sequence, NACK, NACK);
                            }
                        }
                    }
                }
                sequence = (sequence + 1) % 16;
            }
        }
        else if (!strcmp(newCommand->cmd, "get")) {
            t_message *newMessage = buildMessage(newCommand, sequence, GET);

            FILE *f = fopen(newCommand->args[0], "wb");
            if (!f) {
                switch (errno) {
                case ENOMEM:
                    printf("Erro: sem espaço!\n");
                    break;

                case ENOSPC:
                    printf("Erro: sem espaço\n");
                    break;

                case EACCES:
                    printf("Erro: sem permissão.\n");
                    break;
                
                default:
                    break;
                }
            }
            else {
                char response = NACK;
                while (response == NACK) {
                    int send_len = sendMessage(mySocket, newMessage);
                    if(send_len < 0){
                        printf("Erro ao enviar dados para socket.\n");
                    }
                    response = awaitServerResponse(mySocket, &errorCode, sequence);
                }
                if (response == ERROR) {
                    switch(errorCode) {
                        case ARCHIVENOTEXISTANT:
                            printf("Erro: arquivo não existe!\n");
                            break;
                        case WITHOUTPERMISSION:
                            printf("Erro: sem permissão!\n");
                            break;
                        default:
                            printf("Erro!");
                            break;
                    }
                    sequence = (sequence + 1) % 16;
                }
                else if (response == ACK) {
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
                switch (errorCode) {
                case NOSPACE:
                    printf("Erro: sem espaço!\n");
                    break;
                case WITHOUTPERMISSION:
                    printf("Erro: sem permissão!\n");
                    break;
                default:
                    printf("Erro!");
                    break;
                }
                sequence = (sequence + 1) % 16;
            }
            else if (response == OK) {
                long size;
                unsigned char *fileData;
                char res = loadFile(newCommand->args[0], &size, &fileData, &errorCode);
                int send_len;

                if (res == OK) {
                    char c;
                    long indexFile = 0;
                    int i, idx_window_start, seq_window_start;
                    for (i=0; indexFile < size; i++) {
                        if (i%4 == 0) {
                            idx_window_start = indexFile;
                            seq_window_start = sequence;
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
                        res = awaitServerResponse(mySocket, &c, sequence);
                        if (res == NACK) {
                            sequence = seq_window_start;
                            indexFile = idx_window_start;
                        }
                    }
                    printf("Arquivo enviado com sucesso!\n");
                }
                else {
                    switch (errorCode) {
                    case WITHOUTPERMISSION:
                        printf("Erro: sem permissão.\n");
                        break;
                    case ENOENT:
                        printf("Erro: arquivo não existe.\n");
                        break;
                    default:
                        printf("Erro!\n");
                        break;
                    }
                }
                sequence = (sequence + 1) % 16;
                send_len = sendOkErrorResponse(mySocket, sequence, END, END);
                if(send_len < 0){
                    printf("Erro ao enviar dados para socket.\n");
                }
                sequence = (sequence + 1) % 16;
            }
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
                sequence = (sequence + 1) % 16;
            }
            else if (response == OK) {
                printf("Diretório foi criado com sucesso.\n");
                sequence = (sequence + 1) % 16;
            }
            
        }
        else {
            printf("Erro: comando desconhecido!\n");
        }
    }
    
    return 0;
}

