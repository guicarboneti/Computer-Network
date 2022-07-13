#define HOMECLIENT "/home/leon"
#define HOMESERVER "/home/leon"

#define STARTMARKER 126
#define CD 6
#define OK 1
#define NACK 2
#define ERROR 17
#define LS 7

typedef struct {
    unsigned char marker:8;
    unsigned char size:6;
    unsigned char sequence:4;
    unsigned char type:6;
    unsigned char * data;
    unsigned char parity:8;
} t_message