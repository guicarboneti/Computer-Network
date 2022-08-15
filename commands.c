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

char lls(t_command *command, char **names) {
    DIR *dp = NULL; 
    struct dirent *dptr = NULL; 
    unsigned int count = 0;
  
    dp = opendir(".");

    if (!dp)
        return DIRECTORYNOTEXISTANT;
    
    if (command->numArgs == 0) {
        for(count = 0; NULL != (dptr = readdir(dp)); count++) { 
            if(dptr->d_name[0] != '.') 
                printf("%s\n",dptr->d_name); 
        }
    }
    else if (strcmp(command->args[0], "-a") == 0) {
        for(count = 0; NULL != (dptr = readdir(dp)); count++)
            printf("%s\n",dptr->d_name); 
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

            printf("%c%c%c%c%c%c%c%c%c %s\n", rUser, wUser, xUser, rGrp, wGrp, xGrp, rOth, wOth, xOth, dptr->d_name);
        }
    }

    return 0;
}