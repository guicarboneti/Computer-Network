#include "messages.h"

void printMessage(t_message *message) {
    printf("marcador: %d\n", (int)message->header.marker);
    printf("tamanho: %d\n", (int)message->header.size);
    printf("sequencia: %d\n", (int)message->header.sequence);
    printf("tipo: %d\n", (int)message->header.type);
    printf("data: %s\n", message->data);
    printf("paridade: %d\n", (int)message->parity);
}

t_message *buildMessage(t_command *command, int sequence, int cmdCode) {
    t_message *newMessage = malloc(sizeof(t_message));
    newMessage->header.marker = STARTMARKER;
    newMessage->header.sequence = sequence - 1;
    newMessage->header.type = cmdCode;

    newMessage->data = calloc(2, sizeof(char));
    int j = 0, i = 0;

    while (i < command->numArgs) {
        if (command->args[i][0] == '-') {
            newMessage->data[j] = command->args[i][1];
            j++;
            newMessage->data[j] = ',';
            j++;
        }
        else {
            int size = strlen(command->args[i]);
            newMessage->data = realloc(newMessage->data, 2 * sizeof(char) + (size+1) * sizeof(char));
            for (int k = 0; k < size; k++) {
                newMessage->data[j] = command->args[i][k];
                j++;
            }
            newMessage->data[j] = ',';
            j++;
        }
        i++;
    }
    newMessage->data[j-1] = '\0';
    newMessage->header.size = j * sizeof(char);

    // implementar cálculo de paridade
    newMessage->parity = 0;

    printMessage(newMessage);
    return newMessage;
}

t_message *receiveMessage(int socket, int expected) {
    t_message *newMessage = malloc(sizeof(t_message));

    int buflen = recv(socket, &newMessage->header, sizeof(t_header_message), 0);
    if (buflen<0) {
        printf("Erro ao receber header\n");
        return NULL;
    }

    if (newMessage->header.marker != STARTMARKER) {
        return NULL;
    }

    int size = newMessage->header.size;
    newMessage->data = malloc(size);

    char *buffer = malloc(size+1);

    buflen = recv(socket, buffer, size+1, 0);
    if (buflen<0) {
        printf("Erro ao receber data\n");
        return NULL;
    }
    int i;
    for (i = 0; i < size; i++) {
        newMessage->data[i] = buffer[i];
    }
    newMessage->parity = buffer[i];

    if (newMessage->header.sequence < expected)
        return receiveMessage(socket, expected);

    printMessage(newMessage);
    return newMessage;

}

int sendMessage(int socket, t_message *message) {

    int sentLen = send(socket, message, sizeof(message)+100, 0);

    return sentLen;
}