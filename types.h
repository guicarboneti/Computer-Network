#ifndef __TYPES__
#define __TYPES__

#define HOMECLIENT "/home/leon"
#define HOMESERVER "/home/leon"

#define STARTMARKER 126

// types
#define RCD 6
#define OK 1
#define NACK 2
#define ERROR 17
#define RLS 7
#define PRINT 63
#define ACK 3
#define RMKDIR 8
#define GET 9
#define FILEDESC 24
#define DATA 32
#define PUT 10
#define END 46

// errors
#define DIRECTORYNOTEXISTANT 'a'
#define WITHOUTPERMISSION 'b'
#define DIRECTORYALREADYEXISTS 'c'
#define ARCHIVENOTEXISTANT 'd'
#define NOSPACE 'e'
#define OTHER 'f'

typedef struct {
    unsigned char marker:8;
    unsigned char size:6;
    unsigned char sequence:4;
    unsigned char type:6;
} t_header_message;

typedef struct {
    t_header_message header;
    unsigned char *data;
    unsigned char parity:8;
} t_message;

typedef struct {
    char *cmd;
    char **args;
    int numArgs;
} t_command;

#endif