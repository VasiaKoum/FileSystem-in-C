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
        cfs_file = cfs_workwith(cfs_file, file, current);
	}
    else if(strncmp(command, (char*)"cfs_mkdir ", 10)==0 && strlen(command)>11) {
        char *check_dirs = &(command[10]);
        char *dirs = strtok(check_dirs, "\n");
        cfs_mkdir(cfs_file, dirs, current);
    }
    else if(strncmp(command, (char*)"cfs_touch ", 10)==0 && strlen(command)>11) {
        char *check_options = &(command[10]);
        char *options = strtok(check_options, "\n");
        cfs_touch(cfs_file, 1, 1, "vasia", current);
    }
    else if(strncmp(command, (char*)"cfs_pwd", 7)==0) {
        cfs_pwd(cfs_file, current);
    }
    else if(strncmp(command, (char*)"cfs_cd ", 7)==0 && strlen(command)>8) {
        char *check_path = &(command[7]);
        char *path = strtok(check_path, "\n");
        cfs_cd(cfs_file, current, path);
    }
    else printf("Wrong command, type again.\n");
    return cfs_file;
}

/**********************************************************************************************************************/

void add_dir_to_path(list_node **current, unsigned int nodeid, unsigned int offset, char *filename){
    list_node *new, *tmp=*current;
    new = malloc(sizeof(list_node));
    new->nodeid = nodeid;
    new->offset = offset;
    strcpy(new->filename, filename);
    if (tmp != NULL) new->parent_dir = tmp;
    else { new->parent_dir=NULL; }
    (*current) = &(*new);
}

void back_to_path(list_node **current, unsigned int nodeid, int back_depth){
    list_node *delete=*current, *tmp=*current;
    bool found = false;
    while((tmp!=NULL) && (!found)){
        if (tmp->nodeid==nodeid) found = true;
        (*current) = &(*(delete->parent_dir));
        free(delete);
        tmp=*current;
    }
}

void print_current_path(list_node **current){
    list_node *tmp=*current;
    char full_path[FILENAME_SIZE], current_path[FILENAME_SIZE];
    memset(full_path, 0, sizeof(char)*FILENAME_SIZE);
    while(tmp!=NULL){
        sprintf(current_path, "/%s%s", tmp->filename, full_path);
        memset(full_path, 0, sizeof(char)*FILENAME_SIZE);
        sprintf(full_path, "%s", current_path);
        memset(current_path, 0, sizeof(char)*FILENAME_SIZE);
        tmp = tmp->parent_dir;
    }
    printf("%s", full_path);
}

/**********************************************************************************************************************/

void add_to_bitmap(int offset, int cfs_file){
    int record_packet = 0, cfs_place = 0, bitmap_byte_place = 0, bitmap_bit_place = 0;
    lseek(cfs_file, 0, SEEK_SET);
    Superblock *superblock = malloc(sizeof(Superblock));
    read(cfs_file, superblock, sizeof(Superblock));

    Bitmap *bitmap = malloc(sizeof(Bitmap));
    read(cfs_file, bitmap, sizeof(Bitmap));
    record_packet = superblock->metadata_size + (superblock->datablocks_size)*DATABLOCK_NUM;

    offset = offset - sizeof(Superblock) - sizeof(Bitmap);
    cfs_place = offset/record_packet;

    bitmap_byte_place = cfs_place/8;
    bitmap_bit_place = cfs_place%8;
    unsigned char one = 1;
    one = one << bitmap_bit_place;
    // printf("offset: %d cfs_place: %d bitmap_bit_place: %d bitmap_byte_place: %d\n",
    // offset, cfs_place, bitmap_bit_place, bitmap_byte_place);
    bitmap->array[bitmap_byte_place] = bitmap->array[bitmap_byte_place]|one;

    lseek(cfs_file, sizeof(Superblock), SEEK_SET);
    write(cfs_file, bitmap, sizeof(Bitmap));

    free(bitmap);
    free(superblock);
}

int get_space(int cfs_file){
    int i = 0, bitmap_bit_place = 0, record_packet = 0, cfs_place = 0, offset = 0;
    lseek(cfs_file, 0, SEEK_SET);

    Superblock *superblock = malloc(sizeof(Superblock));
    read(cfs_file, superblock, sizeof(Superblock));
    record_packet = superblock->metadata_size + (superblock->datablocks_size)*DATABLOCK_NUM;

    Bitmap *bitmap = malloc(sizeof(Bitmap));
    read(cfs_file, bitmap, sizeof(Bitmap));

    while(bitmap->array[i] == 255) i++;

    unsigned char a = bitmap->array[i];
    int j = 0;
    while(a != 0 && j < 8){
        a = a >> 1;
        j++;
    }
    cfs_place = i*8 + j;

    offset = cfs_place*record_packet + sizeof(Superblock) + sizeof(Bitmap);

    free(superblock); free(bitmap);
    return offset;
}

void delete_from_bitmap(int offset,int cfs_file){
    int record_packet = 0, cfs_place = 0, bitmap_byte_place = 0, bitmap_bit_place = 0;
    lseek(cfs_file, 0, SEEK_SET);
    Superblock *superblock = malloc(sizeof(Superblock));
    read(cfs_file, superblock, sizeof(Superblock));

    Bitmap *bitmap = malloc(sizeof(bitmap));
    read(cfs_file, bitmap,sizeof(bitmap));

    record_packet = superblock->metadata_size + (superblock->datablocks_size)*DATABLOCK_NUM;

    offset = offset - sizeof(Superblock) - sizeof(Bitmap);
    cfs_place = offset/record_packet;
    bitmap_byte_place = cfs_place/8;
    bitmap_bit_place = cfs_place%8;
    unsigned char one = 1;
    one = one << bitmap_bit_place;
    one = ~one;
    bitmap->array[bitmap_byte_place] = bitmap->array[bitmap_byte_place]&one;

    lseek(cfs_file, sizeof(Superblock), SEEK_SET);
    write(cfs_file, bitmap, sizeof(bitmap));

    free(bitmap);
    free(superblock);
}

/************************************************************************************************************************/

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
    memset(bitmap.array, 0, BITMAP_SIZE);

    Superblock superblock;
    superblock.datablocks_size = datablock_size;
    superblock.metadata_size = sizeof(MDS);
    superblock.root_mds_offset = sizeof(superblock) + sizeof(bitmap); // offset tou mds
    superblock.latest_nodeid = 0;
    write(cfs_file, &superblock, sizeof(superblock));
    write(cfs_file, &bitmap, sizeof(bitmap));

    MDS root_mds;
    root_mds.nodeid = 0;
    root_mds.offset = superblock.root_mds_offset;
    root_mds.type = 2;
    root_mds.parent_nodeid = -1;
    root_mds.parent_offset = -1;
    strcpy(root_mds.filename, name);
    root_mds.creation_time = time(0); root_mds.access_time = time(0); root_mds.modification_time = time(0);
    root_mds.data.datablocks[0] = root_mds.offset + superblock.metadata_size;
    for(int i = 1; i < DATABLOCK_NUM; i++){
        root_mds.data.datablocks[i] = root_mds.data.datablocks[i-1] + superblock.datablocks_size;
    }
    write(cfs_file, &root_mds, sizeof(root_mds));

    data_type data;
    data.nodeid = 0;
    data.offset = 0;
    memset(data.filename, 0, FILENAME_SIZE);
    data.active = false;

    for(int i = 0; i < DATABLOCK_NUM; i++){
        lseek(cfs_file, root_mds.data.datablocks[i], SEEK_SET);
        for (int j = 0; j < superblock.datablocks_size/(sizeof(data_type)); j++) {
            write(cfs_file, &data, sizeof(data_type));
        }
    }

    add_to_bitmap(sizeof(Superblock)+sizeof(Bitmap), cfs_file);

    close(cfs_file);
    free(name); free(pathname);
}

int cfs_workwith(int cfs_file, char *filename, list_node **current){
    if(cfs_file!=-1) { close(cfs_file); back_to_path(current, 0, -1); }
    if((cfs_file = open(filename, O_RDWR))<0) { perror("Unable to open file."); return -1;}
    else{
        lseek(cfs_file, 0, SEEK_SET);
        unsigned int mds_offset, nodeid, offset;
        char *filename;
        Superblock *superblock = malloc(sizeof(Superblock));
        MDS *mds = malloc(sizeof(MDS));

        read(cfs_file, superblock, sizeof(Superblock));
        mds_offset = superblock->root_mds_offset;

        lseek(cfs_file, mds_offset, SEEK_SET);
        read(cfs_file, mds, sizeof(MDS));
        nodeid = mds->nodeid;
        offset = mds->offset;
        filename = mds->filename;
        add_dir_to_path(current, nodeid, offset, filename);
        // printf("Now working on file %s\n", filename);
        free(superblock); free(mds);
    }
    return cfs_file;
}

void cfs_mkdir(int cfs_file, char *dirnames, list_node **current){
    if(cfs_file>0){
        lseek(cfs_file, 0, SEEK_SET);
        Superblock *superblock = malloc(sizeof(Superblock));
        read(cfs_file, superblock, sizeof(Superblock));

        data_type data;
        MDS mds;
        MDS currentMDS;
        int free_offset;
        char *dirs = dirnames;
        strtok(dirs, " ");
        while(dirs!=NULL){

            lseek(cfs_file, 0, SEEK_SET);
            free_offset = get_space(cfs_file);
            printf("here free_offset %d\n", free_offset);
            superblock->latest_nodeid++;
            mds.nodeid = superblock->latest_nodeid;
            mds.offset = free_offset;
            mds.type = 2;
            mds.parent_nodeid = (*current)->nodeid;
            mds.parent_offset = (*current)->offset;
            strcpy(mds.filename, dirs);
            mds.creation_time = time(0); mds.access_time = time(0); mds.modification_time = time(0);
            mds.data.datablocks[0] = mds.offset + superblock->metadata_size;
            //xaris
            for(int i = 1; i < DATABLOCK_NUM; i++){
                mds.data.datablocks[i] = mds.data.datablocks[i-1] + superblock->datablocks_size;
            }

            lseek(cfs_file, mds.offset, SEEK_SET);
            write(cfs_file, &mds, sizeof(mds));

            lseek(cfs_file, 0, SEEK_SET);
            write(cfs_file, superblock, sizeof(Superblock));

            data.nodeid = 0;
            data.offset = 0;
            memset(data.filename, 0, FILENAME_SIZE);
            data.active = false;

            for(int i = 0; i < DATABLOCK_NUM; i++){
                lseek(cfs_file, mds.data.datablocks[i], SEEK_SET);
                for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++) {
                    write(cfs_file, &data, sizeof(data_type));
                }
            }

            /* update parent blocks */
            lseek(cfs_file, (*current)->offset, SEEK_SET);
            read(cfs_file, &currentMDS, superblock->metadata_size);

            for(int i = 0; i < DATABLOCK_NUM; i++){
                lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++) {
                    read(cfs_file, &data, sizeof(data_type));
                    if(data.active == false){
                        data.nodeid = mds.nodeid;
                        data.offset = mds.offset;
                        strcpy(data.filename, mds.filename);
                        data.active = true;
                        lseek(cfs_file, -sizeof(data_type), SEEK_CUR);
                        write(cfs_file, &data, sizeof(data_type));
                        i = DATABLOCK_NUM;
                        break;
                    }
                }
            }
            add_to_bitmap(mds.offset, cfs_file);
            //mexri edw

            dirs = strtok(NULL, " ");
        }
        // delete_from_bitmap(int offset,int cfs_file);
        free(superblock);
    }
    else printf("Execute first cfs_workwith.\n");
}

void cfs_touch(int cfs_file, bool time_acc, bool time_edit, char *filenames, list_node **current){
    char *files = filenames;
    int fd_current;
    if(cfs_file>0){
        lseek(cfs_file, 0, SEEK_SET);
        Superblock *superblock = malloc(sizeof(Superblock));
        read(cfs_file, superblock, sizeof(Superblock));

        printf("In cfs_touch\n");
        data_type data;
        MDS mds, currentMDS;
        int free_offset;
        strtok(files, " ");
        while(files != NULL){
            bool exists_already = false;
            bool empty_space = false;
            lseek(cfs_file, (*current)->offset, SEEK_SET);
            read(cfs_file, &currentMDS, superblock->metadata_size);
            //check if name exists!
            for(int i = 0; i < DATABLOCK_NUM; i++){
                lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                    read(cfs_file, &data, sizeof(data_type));
                    if(data.active == true){
                        if(strcmp(data.filename, files) == 0){
                            ///options
                              ///EDW VAZOUME TA OPTIONS
                            ///////
                            printf("Name %s exists already!\n",files);
                            exists_already = true;
                            i = DATABLOCK_NUM;
                            break;
                        }
                    }else{
                        empty_space = true;
                    }
                }
            }
            if(!exists_already && empty_space){
                //make mds
                free_offset = get_space(cfs_file);
                printf("here free_offset %d\n", free_offset);
                superblock->latest_nodeid++;
                mds.nodeid = superblock->latest_nodeid;
                mds.offset = free_offset;
                mds.type = 1;
                mds.parent_nodeid = (*current)->nodeid;
                mds.parent_offset = (*current)->offset;
                strcpy(mds.filename, files);
                mds.creation_time = time(0); mds.access_time = time(0); mds.modification_time = time(0);
                mds.data.datablocks[0] = mds.offset + superblock->metadata_size;
                for(int i = 1; i < DATABLOCK_NUM; i++){
                    mds.data.datablocks[i] = mds.data.datablocks[i-1] + superblock->datablocks_size;
                }
                lseek(cfs_file, mds.offset, SEEK_SET);
                write(cfs_file, &mds, sizeof(mds));

                lseek(cfs_file, 0, SEEK_SET);
                write(cfs_file, superblock, sizeof(Superblock));

                //insert in current dir
                for(int i = 0; i < DATABLOCK_NUM; i++){
                    lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                    for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                        read(cfs_file, &data, sizeof(data_type));
                        if(data.active == false){
                            data.nodeid = mds.nodeid;
                            data.offset = mds.offset;
                            strcpy(data.filename, mds.filename);
                            data.active = true;
                            lseek(cfs_file, -sizeof(data_type), SEEK_CUR);
                            write(cfs_file, &data, sizeof(data_type));
                            i = DATABLOCK_NUM;
                            break;
                        }
                    }
                }
            }
            files = strtok(NULL, " ");
        }
        free(superblock);
    }
    else printf("Execute first cfs_workwith.\n");
}

void cfs_pwd(int cfs_file, list_node **current){
    if(cfs_file>0){
        print_current_path(current);
        printf("\n");
    }
    else printf("Execute first cfs_workwith.\n");
}

void cfs_cd(int cfs_file, list_node **current, char *path){
    char *full_path = path;
    strtok(full_path, "/");
    while(full_path!=NULL){
        printf("dir: %s\n", full_path);
        full_path = strtok(NULL, "/");
    }
}
void cfs_ls(int cfs_file, list_node **current, char *path){
    char *full_path = path;
    strtok(full_path, "/");
    while(full_path!=NULL){
        printf("dir: %s\n", full_path);
        full_path = strtok(NULL, "/");
    }
}

int find_path(int cfs_file, list_node **current, char *path){

}
