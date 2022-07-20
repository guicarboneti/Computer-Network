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
    newMessage->header = malloc(sizeof(t_header_message));

    // maximum size of message
    char *buffer = malloc(67);

    int buflen = recv(socket, buffer, 67, 0);
    if (buflen < 0) {
        printf("Erro ao receber dados\n");
        return NULL;
    }

    // building message from buffer
    memcpy(newMessage->header, buffer, 3);
    if (newMessage->header.marker != STARTMARKER)
        return NULL;

    newMessage->data = malloc(newMessage->header.size);
    int i;
    for (i = 0; i < newMessage->header.size; i++)
        newMessage->data[i] = buffer[i+3];

    newMessage->parity = buffer[i+3];

    // in case received doubly messages
    if (newMessage->header.sequence < expected) {
        free(buffer);
        return receiveMessage(socket, expected);
    }

    // treat bigger than expected

    // check parity

    free(buffer);
    printMessage(newMessage);

    return newMessage;

}

int sendMessage(int socket, t_message *message) {
    //int sentLen = send(socket, message, sizeof(message)+100, 0);
    char *buffer = malloc(4 + sizeof(message->data));

    // copies header to buffer (it occupies 3 bytes)
    buffer = memcpy(buffer, message->header, 3);

    // copies data to buffer
    int i;
    for (i = 3; i < sizeof(message->data) + 3; i++)
        buffer[i] = message->data[i-3];
    
    buffer[i] = message->parity;

    // send (maximum message size is 67)
    int sendLen = send(socket, buffer, 67, 0);
    free(buffer);

    return sentLen;
}