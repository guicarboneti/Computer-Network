#include "messages.h"

void printMessage(t_message *message) {
    printf("marcador: %d\n", (int)message->marker);
    printf("tamanho: %d\n", (int)message->size);
    printf("sequencia: %d\n", (int)message->sequence);
    printf("tipo: %d\n", (int)message->type);
    printf("data:\n");
    for (int i = 0; i < ((int)message->size / sizeof(char)); i++) {
        printf("%c", message->data[i]);
    }
    printf("\n");
    printf("paridade: %d\n", (int)message->parity);
}

t_message *buildMessage(t_command *command, int sequence, int cmdCode) {
    t_message *newMessage = malloc(sizeof(t_message));
    newMessage->marker = STARTMARKER;
    newMessage->sequence = sequence - 1;
    newMessage->type = cmdCode;

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

    newMessage->size = j * sizeof(char);

    // implementar cálculo de paridade
    newMessage->parity = 0;

    return newMessage;

}