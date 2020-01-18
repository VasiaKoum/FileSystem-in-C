#include <stdio.h>      // to have access to printf ()
#include <stdlib.h>     // to enable exit calls
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>      // to have access to flags def
#include <unistd.h>
#include <time.h>
#include "commands.h"
#define PERMS 0644      // set access permissions
#define FILENAME_SIZE 200
#define BLOCK_SIZE 512
#define MAX_FILE_SIZE 3000
#define MAX_FILES_PER_DIR 10


int edit_commands(char *command,int cfs_file, list_node **current){
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
        cfs_file = cfs_workwith(file, current);
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

void add_dir_to_path(list_node **current, unsigned int nodeid, unsigned int offset){
    list_node *new, *tmp=*current;
    new = malloc(sizeof(list_node));
    new->nodeid = nodeid;
    new->offset = offset;
    if (tmp != NULL) new->parent_dir = tmp;
    else { new->parent_dir=NULL; }
    (*current) = &(*new);
}

void back_to_path(list_node **current, unsigned int nodeid){
    list_node *delete=*current, *tmp=*current;
    if (tmp!=NULL){
        while((tmp!=NULL) || (tmp->nodeid!=nodeid)){
            (*current) = &(*(delete->parent_dir));
            free(delete);
            tmp=*current;
        }
    }
}

/**********************************************************************************************************************/

void cfs_create(char* cfs_name, int datablock_size, int filenames_size, int max_file_size, int max_files_in_dirs){
    // printf("In cfs_create with: %s %d %d %d %d\n", cfs_name, datablock_size, filenames_size, max_file_size, max_files_in_dirs);
    int cfs_file;

    // Fix the cfs_name with .cfs prefix
    char *name = malloc(sizeof(char)*strlen(cfs_name)+5);
    char *pathname = malloc(sizeof(char)*strlen(cfs_name)+7);
    strcpy(name, cfs_name); strcat(name, ".cfs");
    // strcpy(pathname, "./"); strcat(pathname, name);

    // Create the cfs file
    if((cfs_file = open(name, O_CREAT|O_RDWR,PERMS))<0){
        perror("Unable to create file.");
        exit(1);
    }
    lseek(cfs_file, 0, SEEK_SET);

    // char *path = realpath(pathname, NULL);
    Bitmap bitmap;
    bitmap.array = malloc(sizeof(char)*1);
    memset(bitmap.array, 0, 1);

    Superblock superblock;
    superblock.datablocks_size = datablock_size;
    superblock.metadata_size = sizeof(MDS);
    superblock.root_mds_offset = sizeof(superblock) + sizeof(bitmap); // offset tou mds
    write(cfs_file, &superblock, sizeof(superblock));
    write(cfs_file, &bitmap, sizeof(bitmap));

    MDS root_mds;
    root_mds.nodeid = 0;
    root_mds.offset = superblock.root_mds_offset;
    root_mds.type = 2;
    root_mds.parent_nodeid = -1;
    root_mds.creation_time = time(0); root_mds.access_time = time(0); root_mds.modification_time = time(0);
    root_mds.data.datablocks[0] = root_mds.offset + superblock.metadata_size;
    for(int i = 1; i < DATABLOCK_NUM; i++){
        root_mds.data.datablocks[i] = root_mds.data.datablocks[i-1] + superblock.datablocks_size;
    }
    write(cfs_file, &root_mds, sizeof(root_mds));
    close(cfs_file);

    free(name); free(pathname);
}

int cfs_workwith(char *filename, list_node **current){
    int cfs_file=-1;
    if((cfs_file = open(filename, O_RDONLY))<0) perror("Unable to open file.");
    else{
        lseek(cfs_file, 0, SEEK_SET);
        unsigned int mds_offset, nodeid, offset;
        Superblock *superblock = malloc(sizeof(Superblock));
        MDS *mds = malloc(sizeof(MDS));
        read(cfs_file, superblock, sizeof(Superblock));
        mds_offset = superblock->root_mds_offset;

        lseek(cfs_file, mds_offset, SEEK_SET);
        read(cfs_file, mds, sizeof(MDS));
        nodeid = mds->nodeid;
        offset = mds->offset;
        add_dir_to_path(current, nodeid, offset);

        printf("Now working on %s file\n", filename);
        free(superblock); free(mds);
    }
    return cfs_file;
}

void cfs_mkdir(char *dirnames, int cfs_file){
    if(cfs_file>0){
        printf("In cfs_mkdir\n");
    }
    else printf("Execute first cfs_workwith.\n");
}

void cfs_touch(bool time_acc, bool time_edit, char *filenames, int cfs_file){
    int fd_current;
    if(cfs_file>0){
      
        printf("In cfs_touch\n");

        ///////1.vlepw pou eimai stin LISTA
        ///////2.tsekarw ean uparxei sto dir to filename[0](1o filename)
        //3.ean den uparxei
            //if xwraei
                // MDS mds;
                // MDS root_mds;
                // root_mds.nodeid = 0;
                // root_mds.offset = superblock.root_mds_offset;
                // root_mds.type = 2;
                // root_mds.parent_nodeid = -1;
                // root_mds.creation_time = time(0); root_mds.access_time = time(0); root_mds.modification_time = time(0);
                // root_mds.data.datablocks[0] = root_mds.offset + superblock.metadata_size;
                // for(int i = 1; i < DATABLOCK_NUM; i++){
                //     root_mds.data.datablocks[i] = root_mds.data.datablocks[i-1] + superblock.datablocks_size;
                // }
                // write(cfs_file, &root_mds, sizeof(root_mds));



    }
    else printf("Execute first cfs_workwith.\n");
}

void cfs_pwd(int cfs_file){
    if(cfs_file>0){
        printf("In cfs_pwd\n");
    }
    else printf("Execute first cfs_workwith.\n");
}
