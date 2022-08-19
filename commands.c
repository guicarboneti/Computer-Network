#include "commands.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

char lcd(t_command *command) {
    int result = chdir(command->args[0]);

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

char lls(t_command *command, int *size, char **names) {
    DIR *dp = NULL; 
    struct dirent *dptr = NULL; 
    unsigned int count = 0;
  
    dp = opendir(".");
    int arraysize = 20;
    names = malloc(arraysize * sizeof(char *));

    if (!dp)
        return DIRECTORYNOTEXISTANT;
    
    if (command->numArgs == 0) {
        for(count = 0; NULL != (dptr = readdir(dp)); count++) { 
            if(dptr->d_name[0] != '.')  {
                names[count] = malloc(63 * sizeof(char));
                sprintf(names[count], "%s",dptr->d_name);
                if ((count % 20) == 0) {
                    arraysize+=20;
                    names = realloc(names, arraysize * sizeof(char *));
                }
            }
        }
    }
    else if (strcmp(command->args[0], "-a") == 0) {
        for(count = 0; NULL != (dptr = readdir(dp)); count++) {
            names[count] = malloc(63 * sizeof(char));
            sprintf(names[count], "%s",dptr->d_name);
            if ((count % 20) == 0) {
                arraysize+=20;
                names = realloc(names, arraysize * sizeof(char *));
            }
        }
    }
    else if (strcmp(command->args[0], "-l") == 0) {
        for(count = 0; NULL != (dptr = readdir(dp)); count++) {
            struct stat *fileStat;
            stat(dptr->d_name, fileStat);

            /* Getting permissions */
            char rUser, wUser, xUser, rGrp, wGrp, xGrp, rOth, wOth, xOth;

            rUser = (fileStat->st_mode & S_IRUSR) ? 'r' : '-';
            wUser = (fileStat->st_mode & S_IWUSR) ? 'w' : '-';
            xUser = (fileStat->st_mode & S_IXUSR) ? 'x' : '-';

            rGrp = (fileStat->st_mode & S_IRGRP) ? 'r' : '-';
            wGrp = (fileStat->st_mode & S_IWGRP) ? 'w' : '-';
            xGrp = (fileStat->st_mode & S_IXGRP) ? 'x' : '-';

            rOth = (fileStat->st_mode & S_IROTH) ? 'r' : '-';
            wOth = (fileStat->st_mode & S_IWOTH) ? 'w' : '-';
            xOth = (fileStat->st_mode & S_IXOTH) ? 'x' : '-';

            names[count] = malloc(63 * sizeof(char));
            sprintf(names[count], "%c%c%c%c%c%c%c%c%c %s", rUser, wUser, xUser, rGrp, wGrp, xGrp, rOth, wOth, xOth, dptr->d_name);
            if ((count % 20) == 0) {
                arraysize+=20;
                names = realloc(names, arraysize * sizeof(char *));
            }
        }
    }
    else {
        for(count = 0; NULL != (dptr = readdir(dp)); count++) { 
            if(dptr->d_name[0] != '.')  {
                names[count] = malloc(63 * sizeof(char));
                sprintf(names[count], "%s",dptr->d_name);
                if ((count % 20) == 0) {
                    arraysize+=20;
                    names = realloc(names, arraysize * sizeof(char *));
                }
            }
        }
    }

    *size = count;

    return OK;
}