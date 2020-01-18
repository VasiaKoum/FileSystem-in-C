#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "commands.h"
#define INPUT_BUF 200

//cfs_create -bs 300 -fns 400 -cfs 100 -mdfn 100 vasia

int main(){
    int cfs_file=-1;
    char input[INPUT_BUF];
    printf(">"); fgets(input, INPUT_BUF, stdin);
    while(strcmp(input, "exit\n")){
        cfs_file = edit_commands(input, cfs_file);
        memset(input, 0, INPUT_BUF);
        printf(">"); fgets(input, INPUT_BUF, stdin);
    }
}
