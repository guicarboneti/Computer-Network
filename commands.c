#include "commands.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

char lcd(char *dir) {
    int result = chdir(dir);

    if (result == -1) {
        switch (errno) {
        case EACCES:
            return WITHOUTPERMISSION;
            break;
        case ENOTDIR:
            return DIRECTORYNOTEXISTANT;
            break;
        case ENOENT:
            return DIRECTORYNOTEXISTANT;
            break;
        default:
            return OTHER;
            break;
        }
    }
    else
        return OK;
}

char lls(char *arg, int *size, char ***names) {
    DIR *dp = NULL; 
    struct dirent *dptr = NULL; 
    unsigned int count = 0;
  
    dp = opendir(".");
    int arraysize = 20;
    char **dirNames = malloc(arraysize * sizeof(char *));

    if (!dp)
        return DIRECTORYNOTEXISTANT;
    
    if (strcmp(arg, "") == 0) {
        for(int i = 0; NULL != (dptr = readdir(dp)); i++) { 
            if(dptr->d_name[0] != '.')  {
                dirNames[count] = malloc(63 * sizeof(char));
                sprintf(dirNames[count], "%s", dptr->d_name);
                count++;
                if ((count % 20) == 0) {
                    arraysize+=20;
                    dirNames = realloc(dirNames, arraysize * sizeof(char *));
                }
            }
        }
    }
    else if (strcmp(arg, "-a") == 0) {
        for(int i = 0; NULL != (dptr = readdir(dp)); i++) {
            dirNames[count] = malloc(63 * sizeof(char));
            sprintf(dirNames[count], "%s",dptr->d_name);
            count++;
            if ((count % 20) == 0) {
                arraysize+=20;
                dirNames = realloc(dirNames, arraysize * sizeof(char *));
            }
        }
    }
    else if (strcmp(arg, "-l") == 0) {
        for(int i = 0; NULL != (dptr = readdir(dp)); i++) {
            if(dptr->d_name[0] != '.')  {
                struct stat fileStat;
                if (stat(dptr->d_name, &fileStat) == -1)
                    return OTHER;

                /* Getting permissions */
                char isDir, rUser, wUser, xUser, rGrp, wGrp, xGrp, rOth, wOth, xOth;

                isDir = (S_ISDIR(fileStat.st_mode)) ? 'd' : '-';

                rUser = (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
                wUser = (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
                xUser = (fileStat.st_mode & S_IXUSR) ? 'x' : '-';

                rGrp = (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
                wGrp = (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
                xGrp = (fileStat.st_mode & S_IXGRP) ? 'x' : '-';

                rOth = (fileStat.st_mode & S_IROTH) ? 'r' : '-';
                wOth = (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
                xOth = (fileStat.st_mode & S_IXOTH) ? 'x' : '-';

                dirNames[count] = malloc(63 * sizeof(char));
                sprintf(dirNames[count], "%c%c%c%c%c%c%c%c%c%c %s", isDir, rUser, wUser, xUser, rGrp, wGrp, xGrp, rOth, wOth, xOth, dptr->d_name);
                count++;
                if ((count % 20) == 0) {
                    arraysize+=20;
                    dirNames = realloc(dirNames, arraysize * sizeof(char *));
                }
            }
        }
    }

    *size = count;
    *names = dirNames;

    return OK;
}

char lmkdir(char *name) {
    int result = mkdir(name, 0777);

    if (result == -1) {
        switch(errno) {
            case EACCES:
                return WITHOUTPERMISSION;
                break;
            case EEXIST:
                return DIRECTORYALREADYEXISTS;
                break;
            default:
                return OTHER;
                break;
        }
    }
    else 
        return OK;
}

<<<<<<< HEAD
char loadFile(char *fileName, long *size, unsigned char **fileData, char *errorCode) {
    FILE *f = fopen(fileName, "rb");

    if (!f) {
        switch(errno) {
            case EACCES:
                return WITHOUTPERMISSION;
                break;
            case ENOENT:
                return ARCHIVENOTEXISTANT;
                break;
            default:
                return OTHER;
                break;
        }
        return ERROR;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize);
    fread(string, fsize, 1, f);
    fclose(f);

    *size = fsize;
    *fileData = string;    

    return OK;
=======
unsigned char **loadFile(char *filename, int *blocks, char *res) {

    FILE *fd = fopen(filename, "r");
    if (!fd) {
        switch (errno) {
        case EACCES:
            *res = WITHOUTPERMISSION;
            break;
        
        default:
            *res = OTHER;
            break;
        }
        return 0;
    }

    int i, length, max_buf_len=63;
    fseek (fd, 0, SEEK_END);
    length = ftell (fd);
    fseek (fd, 0, SEEK_SET);

    double n_blocks = (double)length/(double)max_buf_len;
    unsigned char **fileData = (unsigned char **)malloc(ceil(n_blocks) * sizeof(unsigned char *));
    for(i = 0; i < ceil(n_blocks); i++)
        fileData[i] = (unsigned char *)malloc(max_buf_len * sizeof(unsigned char));

    int bytesRead=0, bufSize;
    if (length < max_buf_len)
        bufSize = length;
    else bufSize = max_buf_len;
    unsigned char *buffer = malloc(bufSize*sizeof(unsigned char));

    for (i=0; bytesRead < length; i++) {
        fread(buffer, 1, bufSize, fd);
        memcpy(fileData[i], buffer, bufSize);
        bytesRead+=bufSize;
        if (bytesRead+bufSize > length) {
            bufSize = length - bufSize;
            memset(buffer, 0, max_buf_len);
        }
    }

    *blocks = (int)ceil(n_blocks);
    fclose (fd);

    *res = OK;
    return fileData;
>>>>>>> d61be8a928f269571416a3c88492fef63ca1b9b9
}