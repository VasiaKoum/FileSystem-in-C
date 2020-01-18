#include <stdio.h>      // to have access to printf ()
#include <stdlib.h>     // to enable exit calls
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>      // to have access to flags def
#include <unistd.h>
#include "commands.h"
#include "structs.h"
#define PERMS 0644      // set access permissions
#define FILENAME_SIZE 200
#define BLOCK_SIZE 512
#define MAX_FILE_SIZE 1000
#define MAX_FILES_PER_DIR 10


int edit_commands(char *command,int cfs_file){
    if (strncmp(command, (char*)"cfs_create ", 11)==0 && strlen(command)>12) {
		char *check_options = &(command[11]);
		char *options = strtok(check_options, "\n");
        char *flags = strtok(options, "-");
        char name[FILENAME_SIZE], *flag_name;
        memset(name, 0, FILENAME_SIZE);
        int bs=BLOCK_SIZE, fns=FILENAME_SIZE, cfs=MAX_FILE_SIZE, mdfn=MAX_FILES_PER_DIR;
        while(flags!=NULL){
            if(strncmp(flags, (char*)"bs", 2)==0) sscanf(flags,"%*s %d %s",&bs, name);
            else if(strncmp(flags, (char*)"cfs", 3)==0) sscanf(flags,"%*s %d %s",&cfs, name);
            else if(strncmp(flags, (char*)"fns", 3)==0) sscanf(flags,"%*s %d %s",&fns, name);
            else if(strncmp(flags, (char*)"mdfn", 4)==0) sscanf(flags,"%*s %d %s",&mdfn, name);
            flags = strtok(NULL, "-");
        }
        if (strlen(name)>0) cfs_create(name, bs, fns, cfs, mdfn);
        else printf("Name for cfs_file is missing.\n");
        return -1;
	}
    else if(strncmp(command, (char*)"cfs_workwith ", 13)==0 && strlen(command)>14) {
		char *check_file = &(command[13]);
		char *file = strtok(check_file, "\n");
        cfs_file = cfs_workwith(file);
	}
    else if(strncmp(command, (char*)"cfs_mkdir ", 10)==0 && strlen(command)>11) {
        char *check_dirs = &(command[10]);
        char *dirs = strtok(check_dirs, "\n");
        cfs_mkdir(dirs, cfs_file);
    }
    else if(strncmp(command, (char*)"cfs_touch ", 10)==0 && strlen(command)>11) {
        char *check_options = &(command[10]);
        char *options = strtok(check_options, "\n");
        cfs_touch(1, 1, "vasia", cfs_file);
    }
    else if(strncmp(command, (char*)"cfs_pwd", 7)==0 ) {
        cfs_pwd(cfs_file);
    }
    else printf("Wrong command, type again.\n");
    return cfs_file;
}

/**********************************************************************************************************************/

void cfs_create(char* cfs_name, int datablock_size, int filenames_size, int max_file_size, int max_files_in_dirs){
    // printf("In cfs_create with: %s %d %d %d %d\n", cfs_name, datablock_size, filenames_size, max_file_size, max_files_in_dirs);
    int cfs_file;

    // Fix the cfs_name with .cfs prefix
    char *name = malloc(sizeof(char)*strlen(cfs_name)+5);
    char *pathname = malloc(sizeof(char)*strlen(cfs_name)+7);
    strcpy(name, cfs_name); strcat(name, ".cfs"); strcpy(pathname, "./"); strcat(pathname, name);

    // Create the cfs file
    if((cfs_file = open(name, O_CREAT|O_RDWR,PERMS))<0){
        perror("Unable to create file.");
        exit(1);
    }
    lseek(cfs_file, 0, SEEK_SET);

    char *path = realpath(pathname, NULL);
    Superblock superblock;
    superblock.datablocks_size = datablock_size;
    superblock.metadata_size = sizeof(MDS);
    strcpy(superblock.path_root, path);
    write(cfs_file, &superblock, sizeof(superblock));

    Bitmap bitmap;
    bitmap.array = malloc(sizeof(char)*1);
    memset(bitmap.array, 0, 1);
    write(cfs_file, &bitmap, sizeof(bitmap));

    /* Initilize cfs_file->MDS */
    //lseek()
    MDS route_mds;
    route_mds.nodeid = 0;
    route_mds.offset =



    close(cfs_file);

    free(name); free(pathname);
}

int cfs_workwith(char *filename){
    int cfs_file=-1;
    if((cfs_file = open(filename, O_RDONLY))<0) perror("Unable to open file.");

    /* EDW THA ANANEWSOUME TIS DOMES POU THA FTIAXOUME DLD THN LISTA */

    // close(cfs_file);
    printf("Now working on %s file\n", filename);
    return cfs_file;
}

void cfs_mkdir(char *dirnames, int cfs_file){
    if(cfs_file>0){
        printf("In cfs_mkdir\n");
    }
    else printf("Execute first cfs_workwith.\n");
}

void cfs_touch(bool time_acc, bool time_edit, char *filenames, int cfs_file){
    if(cfs_file>0){
        printf("In cfs_touch\n");
    }
    else printf("Execute first cfs_workwith.\n");
}

void cfs_pwd(int cfs_file){
    if(cfs_file>0){
        printf("In cfs_pwd\n");
    }
    else printf("Execute first cfs_workwith.\n");
}
