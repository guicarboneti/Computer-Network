#define HOMECLIENT "/home/leon"
#define HOMESERVER "/home/leon"

#define STARTMARKER 126
#define RCD 6
#define OK 1
#define NACK 2
#define ERROR 17
#define RLS 7
#define PRINT 63
#define ACK 3
#define END 46
#define RMKDIR 8
#define GET 9
#define FILEDESC 24
#define DATA 32
#define PUT 10

typedef struct {
    unsigned char marker:8;
    unsigned char size:6;
    unsigned char sequence:4;
    unsigned char type:6;
    unsigned char * data;
    unsigned char parity:8;
} t_message;