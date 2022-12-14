#include "messages.h"
#include "utils.h"

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
    newMessage->header.sequence = sequence;
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
    newMessage->parity = calculateParity(newMessage);

    //printMessage(newMessage);
    return newMessage;
}

t_message *receiveMessage(int socket) {
    t_message *newMessage = malloc(sizeof(t_message));

    // maximum size of message
    char *buffer = malloc(68);

    struct timeval tv;
    tv.tv_sec = 20;
    tv.tv_usec = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    int buflen = recv(socket, buffer, 68, 0);
    if (buflen < 0) {
        printf("Erro ao receber dados\n");
        return NULL;
    }

    // building message from buffer
    memcpy(&newMessage->header, buffer, 4);
    if (newMessage->header.marker != STARTMARKER)
        return NULL;

    newMessage->data = malloc(newMessage->header.size);
    int i;
    for (i = 0; i < newMessage->header.size; i++)
        newMessage->data[i] = buffer[i+4];

    newMessage->parity = buffer[i+4];

    // treat bigger than expected

    free(buffer);
    return newMessage;

}

int sendMessage(int socket, t_message *message) {
    //int sentLen = send(socket, message, sizeof(message)+100, 0);
    char *buffer = malloc(5 + message->header.size);

    // copies header to buffer (it occupies 3 bytes)
    buffer = memcpy(buffer, &message->header, 4);

    // copies data to buffer
    int i;
    for (i = 4; i < message->header.size + 4; i++)
        buffer[i] = message->data[i-4];
    
    buffer[i] = message->parity;

    // send (maximum message size is 67)
    int sendLen = send(socket, buffer, 68, 0);

    free(buffer);
    return sendLen;
}

int sendNack(int socket, t_message *message) {
    message->header.type = NACK;
    return sendMessage(socket, message);
}

char awaitServerResponse(int socket, char *errorCode, int sequence) {
	while(1) {
		t_message *response = receiveMessage(socket);
        if (response != NULL) {
            if (response->header.marker == STARTMARKER && response->header.sequence == sequence && ((response->header.type == NACK) || (response->header.type == ACK) || (response->header.type == OK) || (response->header.type == ERROR))) {
                if (response->header.size > 0)
                    *errorCode = response->data[0];
                return response->header.type;
            }
        }
        else
            return NACK;
	}
}

int calculateParity (t_message *message) {
    int i, j, bits[8], parity[8] = {0};
    for (i=0; i<message->header.size; i++) {    // each char in data
        charToBinary(bits, message->data[i]);
        for (j=0; j<8; j++) {                   // each bit in data char
            if (bits[j] == '1')
                parity[j]++;                    // store sum of 1's in position j for each char
        }
    }

    for (i=0; i<8; i++) {                       // calculate parity and store it in array bits
        if (parity[i]%2 == 0)
            parity[i] = 0;
        else
            parity[i] = 1;
    }


    return binaryToDecimal(parity); 
}

int compareParity(char parity1, char parity2) {
    int i;
    int bin_msg1[8], bin_msg2[8];
    charToBinary(bin_msg1, parity1);
    charToBinary(bin_msg2, parity2);

    for (i=0; i<8; i++) {
        if (bin_msg1[i] != bin_msg2[i])
            return 0;
    }
    return 1;
}

int sendOkErrorResponse(int socket, int sequence, char status, char response) {
    t_message *newMessage = malloc(sizeof(t_message));
    newMessage->data = malloc(sizeof(char));

    //builds header
    newMessage->header.marker = STARTMARKER;
    newMessage->header.sequence = sequence;
    newMessage->header.size = 1;
    newMessage->header.type = status;

    newMessage->data[0] = response;

    newMessage->parity = calculateParity(newMessage);
    
    int sendLen = sendMessage(socket, newMessage);

    free(newMessage);

    return sendLen;
}