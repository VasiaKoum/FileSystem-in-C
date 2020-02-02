#include <stdio.h>      // to have access to printf ()
#include <stdlib.h>     // to enable exit calls
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>      // to have access to flags def
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
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
        int i=0;
        strcpy(name, flags);
        int bs=BLOCK_SIZE, fns=FILENAME_SIZE, cfs=MAX_FILE_SIZE, mdfn=MAX_FILES_PER_DIR;
        while(flags!=NULL){
            if(i == 1) memset(name, 0, FILENAME_SIZE);
            if(strncmp(flags, (char*)"bs", 2)==0) sscanf(flags,"%*s %d %s",&bs, name);
            else if(strncmp(flags, (char*)"cfs", 3)==0) sscanf(flags,"%*s %d %s",&cfs, name);
            else if(strncmp(flags, (char*)"fns", 3)==0) sscanf(flags,"%*s %d %s",&fns, name);
            else if(strncmp(flags, (char*)"mdfn", 4)==0) sscanf(flags,"%*s %d %s",&mdfn, name);
            flags = strtok(NULL, "-");
            i++;
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
        char *options = strtok(command, " \n");
        char *filename=NULL;
        bool time_acc=true, time_edit=false;
        while(options!=NULL){
            if(strcmp(options, (char*)"-a")==0) time_acc=true;
            else if(strcmp(options, (char*)"-m")==0) time_edit=true;
            else { filename = options; }
            options = strtok(NULL, " \n");
        }
        cfs_touch(cfs_file, time_acc, time_edit, filename, current);
    }
    else if(strncmp(command, (char*)"cfs_pwd", 7)==0) {
        cfs_pwd(cfs_file, current);
    }
    else if(strncmp(command, (char*)"cfs_cd ", 7)==0 && strlen(command)>8) {
        char *check_path = &(command[7]);
        char *path = strtok(check_path, "\n");
        cfs_cd(cfs_file, current, path);
    }
    else if(strncmp(command, (char*)"cfs_ls", 6)==0 || (strncmp(command, (char*)"cfs_ls ", 7)==0 && strlen(command)>8)) {
        char *options = strtok(command, " \n");
        bool a=false, r=false, l=false, u=false, d=false, h=false;
        char *filename=NULL;
        while(options!=NULL){
            if(strcmp(options, (char*)"-a")==0) a=true;
            else if(strcmp(options, (char*)"-r")==0) r=true;
            else if(strcmp(options, (char*)"-l")==0) l=true;
            else if(strcmp(options, (char*)"-u")==0) u=true;
            else if(strcmp(options, (char*)"-d")==0) d=true;
            else if(strcmp(options, (char*)"-h")==0) h=true;
            else filename = options;
            options = strtok(NULL, " \n");
        }
        cfs_ls(cfs_file, a, r, l, u, d, l, filename, current, NULL);
    }
    else if(strncmp(command, (char*)"cfs_mv ", 7)==0 && strlen(command)>8) {
        char *check_path = &(command[7]);
        bool i=false;
        char path[FILENAME_SIZE]; memset(path, 0, FILENAME_SIZE);
        strtok(check_path, "\n");
        if(strncmp(command, "cfs_mv -i ", 10)==0 ) { i=true; strncpy(path, check_path+3, strlen(check_path)); }
        else strncpy(path, check_path, strlen(check_path));

        char *tmp=check_path, *destination=check_path, *source=path;
        tmp = strrchr(command, ' ');
        if(tmp!=NULL) {
            destination = tmp+1;
            // running with:
            // cfs_mv -i ./s1 ./s2 ./s3 ./dest
            // cfs_mv ./s1 ./s2 ./s3 ./dest
            printf("option: [%d], source: [%s], destination: [%s]\n", i, source, destination);
            // cfs_mv(cfs_file, current, source, destination, i);
        }
        else printf("cfs_mv: missing file operand\n");
    }
    else if(strncmp(command, (char*)"cfs_rm ", 7)==0 && strlen(command)>8) {
        bool r = false;
        char *check_path = &(command[7]);
        char *path = strtok(check_path, "\n"), *pathname=NULL;

        if(path!=NULL){
            if(strncmp(path, "-r", 2)==0) {
                r = true;
                path+=2;
                while(*path!='\0'){
                    if(*path == ' ') path++;
                    else { pathname = path; break; }
                }
            }
            else pathname = path;
        }
        if(pathname!=NULL) cfs_rm(cfs_file, current, pathname, r);
        else printf("cfs_rm: missing file operand\n");
    }
    else if(strncmp(command, (char*)"cfs_ln ", 7)==0 && strlen(command)>8) {
        char *check_path = &(command[7]);
        char *source = strtok(check_path, " \n"), *output;
        if (source != NULL) {
            output = strtok(NULL, " \n");
            if (output != NULL) cfs_ln(cfs_file, current, source, output);
            else printf("cfs_ln: failed to create hard link '%s'\n", source);
        }
        else printf("cfs_ln: missing file operand\n");
    }
    else if(strncmp(command, (char*)"cfs_cat ", 8)==0 && strlen(command)>9) {
        char *check_path = &(command[7]);
        char *path = strtok(check_path, "\n");
    }
    else if(strncmp(command, (char*)"cfs_cp ", 7)==0 && strlen(command)>8) {
        char *check_path = &(command[7]);
        char *path = strtok(check_path, "\n");
    }
    else if(strncmp(command, (char*)"cfs_import ", 11)==0 && strlen(command)>12) {
        char *check_path = &(command[11]);
        strtok(check_path, "\n");
        char *tmp = strrchr(command, ' '), *directory;
        if(tmp!=NULL) {
            directory = tmp+1;
            // running with:
            // cfs_import ./here/dir1 ./here/new.txt sss
            cfs_import(cfs_file, current, check_path, directory);
        }
        else printf("cfs_import: missing file operand\n");
    }
    else if(strncmp(command, (char*)"cfs_export ", 11)==0 && strlen(command)>12) {
        char *check_path = &(command[11]);
        strtok(check_path, "\n");
        char *tmp = strrchr(command, ' '), *directory;
        if(tmp!=NULL) {
            directory = tmp+1;
            // running with:
            // cfs_export ./dir1/new.txt here
            // cfs_export ./dir1/new.txt ./dir1 ./dir2 here
            // printf("source: [%s], destination: [%s]\n", check_path, destination);
            cfs_export(cfs_file, current, check_path, directory);
        }
        else printf("cfs_export: missing file operand\n");
    }
    else { strtok(command, "\n"); printf("%s: command not found\n", command); }
    return cfs_file;
}

/**********************************************************************************************************************/

void add_dir_to_path(list_node **current, unsigned int nodeid, unsigned int offset, char *filename){
    list_node *new, *tmp=*current;
    new = (list_node *)malloc(sizeof(list_node));
    new->nodeid = nodeid;
    new->offset = offset;
    strncpy(new->filename, filename, FILENAME_SIZE);
    if (tmp != NULL) new->parent_dir = tmp;
    else { new->parent_dir=NULL; }
    (*current) = &(*new);
}

void destroy_list(list_node **current){
    list_node *tmp=*current, *delete=*current;
    while(tmp!=NULL){
        delete = tmp->parent_dir;
        free(tmp);
        tmp = delete;
    }
    (*current) = NULL;
}

void back_to_path(list_node **current, unsigned int nodeid, int back_depth){
    list_node *delete=*current, *tmp=*current;
    bool found = false;
    int i=1;
    while((tmp!=NULL) && (!found)){
        if(back_depth==-1) { if (tmp->nodeid==nodeid) found = true; }
        else { if(i<back_depth) i++; else found = true; }
        (*current) = &(*(delete->parent_dir));
        free(delete);
        tmp=*current;
    }
}

int parent_offset(list_node **current){
    list_node *tmp=*current;
    int par_off=-1;
    if (tmp!=NULL){
        if (tmp->parent_dir!=NULL){
            tmp = tmp->parent_dir;
            par_off = tmp->offset;
        }
    }
    return par_off;
}

void print_current_path(list_node **current){
    list_node *tmp=*current;
    char *full_path=malloc(FILENAME_SIZE*sizeof(char)), *current_path=malloc(FILENAME_SIZE*sizeof(char));
    memset(full_path, 0, sizeof(char)*FILENAME_SIZE);
    while(tmp!=NULL){
        free(current_path); current_path = malloc(strlen(full_path)+strlen(tmp->filename)+2);
        sprintf(current_path, "/%s%s", tmp->filename, full_path);
        free(full_path); full_path = malloc(strlen(current_path)+1);
        sprintf(full_path, "%s", current_path);
        tmp = tmp->parent_dir;
    }
    printf("%s", full_path);
    free(current_path); free(full_path);
}

int find_path(int cfs_file, list_node **current, char *path, bool change_pathlist){
    int current_offset=(*current)->offset;
    bool relative_path=true;

    if(cfs_file>0){
        Superblock *superblock = malloc(sizeof(Superblock));
        MDS mds, currentMDS;
        char str_path[FILENAME_SIZE]; memset(str_path, 0, FILENAME_SIZE); strcpy(str_path, path);
        char *full_path = str_path;
        if(strncmp(full_path, "/", 1)==0) relative_path=false;
        full_path = strtok(str_path, "/");

        while(full_path!=NULL){
            // If path is relative
            if (relative_path){
                if(strcmp(full_path, ".")==0){

                }
                else if(strcmp(full_path, "..")==0){
                    lseek(cfs_file, 0, SEEK_SET);
                    read(cfs_file, superblock, sizeof(Superblock));
                    lseek(cfs_file, current_offset, SEEK_SET);
                    read(cfs_file, &currentMDS, superblock->metadata_size);
                    current_offset = currentMDS.parent_offset;
                    if(change_pathlist) back_to_path(current, 0, 1);
                }
                else{
                    int fd_current, free_offset;
                    data_type data;
                    lseek(cfs_file, 0, SEEK_SET);
                    read(cfs_file, superblock, sizeof(Superblock));
                    bool exists_already = false;
                    lseek(cfs_file, current_offset, SEEK_SET);
                    read(cfs_file, &currentMDS, superblock->metadata_size);
                    //check if name exists!
                    for(int i = 0; i < DATABLOCK_NUM; i++){
                        lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                        for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                            read(cfs_file, &data, sizeof(data_type));
                            if(data.active == true){
                                if(strcmp(data.filename, full_path) == 0){
                                    lseek(cfs_file, data.offset, SEEK_SET);
                                    read(cfs_file, &mds, sizeof(MDS));
                                    if(mds.type == 2){
                                        if(change_pathlist) add_dir_to_path(current, data.nodeid, data.offset, data.filename);
                                    }
                                    else if(change_pathlist) printf("%s: Not a directory\n", full_path);
                                    current_offset = data.offset;
                                    exists_already = true;
                                    i = DATABLOCK_NUM;
                                    break;
                                }
                            }
                        }
                    }
                    if(!exists_already) { free(superblock); return -1; }
                }
            }
            // If path is absolute
            else{
                printf("absolute path\n");
            }
            full_path = strtok(NULL, "/");
        }
        free(superblock);
    }
    return current_offset;
}

int find_file(int cfs_file, char *filename){
    if(cfs_file>0){
        Superblock *superblock = malloc(sizeof(Superblock));
        int fd_current, free_offset, i=0;
        data_type data;
        MDS mds, currentMDS;
        lseek(cfs_file, 0, SEEK_SET);
        read(cfs_file, superblock, sizeof(Superblock));
        bool exists_already = false;

        while(true){
            lseek(cfs_file, sizeof(Superblock)+sizeof(Bitmap)+(i*(superblock->metadata_size+(DATABLOCK_NUM*superblock->datablocks_size))), SEEK_SET);
            if(read(cfs_file, &currentMDS, superblock->metadata_size)>0){
                // int current_pointer = lseek(cfs_file, 0, SEEK_CUR);
                if (strcmp(currentMDS.filename, filename)==0) { free(superblock); return currentMDS.offset; }
            }
            else { free(superblock); return -1; }
            i++;
        }
    }
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
    bitmap->array[bitmap_byte_place] = bitmap->array[bitmap_byte_place]|one;

    // printf("offset: %d cfs_place: %d bitmap_bit_place: %d bitmap_byte_place: %d\n",
    // offset, cfs_place, bitmap_bit_place, bitmap_byte_place);

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
        free(superblock); free(mds);
    }
    return cfs_file;
}

int cfs_mkdir(int cfs_file, char *dirnames, list_node **current){
    int return_offset = -1;
    if(cfs_file>0){
        Superblock *superblock = malloc(sizeof(Superblock));
        int free_offset;
        char *dirs = dirnames;
        strtok(dirs, " ");
        while(dirs!=NULL){
            data_type data;
            MDS mds, currentMDS;

            lseek(cfs_file, 0, SEEK_SET);
            read(cfs_file, superblock, sizeof(Superblock));

            free_offset = get_space(cfs_file);
            return_offset = free_offset;
            superblock->latest_nodeid++;
            mds.nodeid = superblock->latest_nodeid;
            mds.offset = free_offset;
            mds.size = 0;
            mds.type = 2;
            mds.parent_nodeid = (*current)->nodeid;
            mds.parent_offset = (*current)->offset;
            strcpy(mds.filename, dirs);
            mds.creation_time = time(0); mds.access_time = time(0); mds.modification_time = time(0);
            mds.data.datablocks[0] = mds.offset + superblock->metadata_size;

            for(int i = 1; i < DATABLOCK_NUM; i++){
                mds.data.datablocks[i] = mds.data.datablocks[i-1] + superblock->datablocks_size;
            }

            lseek(cfs_file, mds.offset, SEEK_SET);
            write(cfs_file, &mds, superblock->metadata_size);

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

            for(int i = 0; i < DATABLOCK_NUM; i++) {
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
            dirs = strtok(NULL, " ");
        }
        free(superblock);
    }
    else printf("cfs_mkdir: execute first cfs_workwith\n");
    return return_offset;
}

int cfs_touch(int cfs_file, bool time_acc, bool time_edit, char *filenames, list_node **current){
    char *files = filenames;
    int fd_current, return_offset=-1;

    if(cfs_file>0){
        Superblock *superblock = malloc(sizeof(Superblock));
        int free_offset, source_offset;
        strtok(files, " ");

        while(files != NULL){
            data_type data;
            MDS mds, currentMDS, fileMDS;
            lseek(cfs_file, 0, SEEK_SET);
            read(cfs_file, superblock, sizeof(Superblock));
            bool exists_already = false;
            bool empty_space = false;
            lseek(cfs_file, (*current)->offset, SEEK_SET);
            read(cfs_file, &currentMDS, superblock->metadata_size);
            //check if name exists!
            for(int i = 0; i < DATABLOCK_NUM; i++){
                lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                    read(cfs_file, &data, sizeof(data_type));
                    int current_pointer = lseek(cfs_file, 0, SEEK_CUR);
                    if(data.active == true){
                        if(strcmp(data.filename, files) == 0){
                            if(time_acc || time_edit){
                                lseek(cfs_file, data.offset, SEEK_SET);
                                read(cfs_file, &fileMDS, superblock->metadata_size);
                                if(time_acc) fileMDS.access_time = time(0);
                                if(time_edit) fileMDS.modification_time = time(0);
                                lseek(cfs_file, data.offset, SEEK_SET);
                                write(cfs_file, &fileMDS, superblock->metadata_size);
                                lseek(cfs_file, current_pointer, SEEK_SET);
                                return_offset = data.offset;
                            }
                            exists_already = true;
                            i = DATABLOCK_NUM;
                            break;
                        }
                    }
                    else{
                        empty_space = true;
                    }
                }
            }
            if(!exists_already && empty_space){
                //make mds
                free_offset = get_space(cfs_file);
                return_offset = free_offset;
                superblock->latest_nodeid++;
                mds.nodeid = superblock->latest_nodeid;
                mds.size = 0;
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
                write(cfs_file, &mds, superblock->metadata_size);

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
                add_to_bitmap(mds.offset, cfs_file);
            }
            files = strtok(NULL, " ");
        }
        free(superblock);
    }
    else printf("cfs_touch: execute first cfs_workwith\n");
    return return_offset;
}

void cfs_pwd(int cfs_file, list_node **current){
    if(cfs_file>0){
        print_current_path(current);
        printf("\n");
    }
    else printf("cfs_pwd: execute first cfs_workwith\n");
}

void cfs_cd(int cfs_file, list_node **current, char *path){
    if(cfs_file>0) {
        if(find_path(cfs_file, current, path, true)<0)
            printf("cfs_cd: failed to access %s: no such file or directory\n", path);
    }
    else printf("cfs_cd: execute first cfs_workwith\n");
}

void cfs_ls(int cfs_file, bool a, bool r, bool l, bool u, bool d, bool h, char *filename, list_node **current, char *path){
    if(cfs_file>0){
        Superblock *superblock = malloc(sizeof(Superblock));
        char *full_path = path;
        if (path == NULL) full_path = (*current)->filename;
        else strtok(full_path, "/");
        while(full_path!=NULL){
            data_type data;
            MDS currentMDS, fileMDS;

            lseek(cfs_file, 0, SEEK_SET);
            Superblock *superblock = malloc(sizeof(Superblock));
            read(cfs_file, superblock, sizeof(Superblock));

            lseek(cfs_file, (*current)->offset, SEEK_SET);
            read(cfs_file, &currentMDS, superblock->metadata_size);
            //check if name exists!
            for(int i = 0; i < DATABLOCK_NUM; i++){
                lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                    read(cfs_file, &data, sizeof(data_type));
                    int current_pointer = lseek(cfs_file, 0, SEEK_CUR);
                    if(data.active == true){
                        if(a){
                            printf("%s\n",data.filename);
                        }
                        if(l){
                            lseek(cfs_file, data.offset, SEEK_SET);
                            read(cfs_file, &fileMDS, superblock->metadata_size);
                            char create[20], modify[20], access[20];
                            strftime(create, 20, "%F %H:%M:%S", localtime(&fileMDS.creation_time));
                            strftime(modify, 20, "%F %H:%M:%S", localtime(&fileMDS.modification_time));
                            strftime(access, 20, "%F %H:%M:%S", localtime(&fileMDS.access_time));
                            if(fileMDS.type == 2) printf("%s \t DIR \t %d \t  (TC)%s  (TA)%s  (TM)%s\n", data.filename, data.offset, create, access, modify);
                            else if(fileMDS.type == 1) printf("%s \t FILE \t %d \t  (TC)%s  (TA)%s  (TM)%s\n", data.filename, data.offset, create, access, modify);
                            else printf("%s \t LK \t %d \t  (TC)%s  (TA)%s  (TM)%s\n", data.filename, data.offset, create, access, modify);
                            lseek(cfs_file, current_pointer, SEEK_SET);
                        }
                        else{
                            if(strncmp(data.filename, ".", 1)!=0) printf("%s\n",data.filename);
                        }
                    }
                }
            }
            full_path = strtok(NULL, "/");
        }
        free(superblock);
    }
    else printf("cfs_ls: execute first cfs_workwith\n");
}

void cfs_mv(int cfs_file,  list_node **current, char *path_source, char *destination, bool i){
    bool exists = false, option_i = true;
    char all_sources[FILENAME_SIZE];
    strcpy(all_sources, path_source);

    if(cfs_file>0){

        char *source = strtok(all_sources, " ");
        if (source == NULL) source = all_sources;
        printf("source %s, destination %s\n", source, destination);

        int file_offset, dest_offset;
        if((dest_offset = find_path(cfs_file, current, destination, false))<0){
            //printf("cfs_mv: failed to access %s: No such file or directory\n", destination);
            if ((file_offset = find_path(cfs_file, current, source, false))<0){
                printf("cfs_mv: failed to access %s: No such file or directory\n", source);
                source = NULL;
            }else{
                data_type data;
                MDS currentMDS, fileMDS;

                lseek(cfs_file, 0, SEEK_SET);
                Superblock *superblock = malloc(sizeof(Superblock));
                read(cfs_file, superblock, sizeof(Superblock));

                lseek(cfs_file, file_offset, SEEK_SET);
                read(cfs_file, &fileMDS, superblock->metadata_size);

                lseek(cfs_file, fileMDS.parent_offset, SEEK_SET);
                read(cfs_file, &currentMDS, superblock->metadata_size);

                for(int i = 0; i < DATABLOCK_NUM; i++){
                    lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                    for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                        read(cfs_file, &data, sizeof(data_type));
                        if(data.active == true){
                            printf("Name [%s]=[%s] \n",data.filename, source);
                             if(strcmp(data.filename, fileMDS.filename) == 0){

                                 strcpy(data.filename, destination);
                                 lseek(cfs_file, -sizeof(data_type), SEEK_CUR);
                                 write(cfs_file, &data, sizeof(data_type));
                                 i = DATABLOCK_NUM;
                                 break;
                             }
                        }
                    }
                }
                strcpy(fileMDS.filename, destination);
                //fileMDS.modification_time
                lseek(cfs_file, file_offset, SEEK_SET);
                write(cfs_file, &fileMDS, superblock->metadata_size);

            }
        }else{
            while(source!=NULL){
                data_type data,tempdata;
                MDS currentMDS, destMDS, fileMDS;
                printf("source is%s\n", source);
                char answer[5];
                if(i == true){
                    printf("Are you sure to move %s?(y/n)\n",source);
                    scanf("%s\n", answer);
                    if(strcmp(answer,"y")){
                        option_i = true;
                    }else{
                        option_i = false;
                    }
                }
                if ((file_offset = find_path(cfs_file, current, source, false))<0){
                    printf("cfs_mv: failed to access %s: No such file or directory\n", source);
                    source = NULL;
                }
                else if(option_i){
                    lseek(cfs_file, 0, SEEK_SET);
                    Superblock *superblock = malloc(sizeof(Superblock));
                    read(cfs_file, superblock, sizeof(Superblock));

                    lseek(cfs_file, dest_offset, SEEK_SET);
                    read(cfs_file, &destMDS, superblock->metadata_size);

                    if(destMDS.type == 2){

                        lseek(cfs_file, file_offset, SEEK_SET);
                        read(cfs_file, &fileMDS, superblock->metadata_size);

                        lseek(cfs_file, fileMDS.parent_offset, SEEK_SET);
                        read(cfs_file, &currentMDS, superblock->metadata_size);
                        //check if name exists!
                        for(int i = 0; i < DATABLOCK_NUM; i++){
                            lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                            for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                                read(cfs_file, &data, sizeof(data_type));
                                if(data.active == true){
                                    printf("Name [%s]=[%s] \n",data.filename, source);
                                    if(strcmp(data.filename, fileMDS.filename) == 0){
                                        exists = true;
                                        tempdata.nodeid = data.nodeid;
                                 	    tempdata.offset = data.offset;
                                 	    strcpy(tempdata.filename, data.filename);
                                 	    tempdata.active = true;
                                        data.active = false;
                                        lseek(cfs_file, -sizeof(data_type), SEEK_CUR);
                                        write(cfs_file, &data, sizeof(data_type));
                                        i = DATABLOCK_NUM;
                                        break;
                                    }
                                }
                            }
                        }

                        if(exists){
                            for(int i = 0; i < DATABLOCK_NUM; i++){
                                lseek(cfs_file, destMDS.data.datablocks[i], SEEK_SET);
                                for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                                    read(cfs_file, &data, sizeof(data_type));
                                    if(data.active == false){
                                        lseek(cfs_file, -sizeof(data_type), SEEK_CUR);
                                        write(cfs_file, &tempdata, sizeof(data_type));
                                        fileMDS.parent_offset = dest_offset;
                                        lseek(cfs_file, file_offset, SEEK_SET);
                                        write(cfs_file, &fileMDS, superblock->metadata_size);
                                        i = DATABLOCK_NUM;
                                        break;
                                    }
                                }
                            }
                        }
                        source = strtok(NULL, " ");
                        free(superblock);
                    }else{
                        printf("destination %s not a dir",destination);
                        source = strtok(NULL, " ");
                        free(superblock);
                    }
                }
            }
        }
    }
    else printf("cfs_mv: execute first cfs_workwith\n");
}

void cfs_rm(int cfs_file,  list_node **current, char *path, bool r){
    bool deleted = false;
    if(cfs_file>0){
        char full_path[FILENAME_SIZE];
        int dest_offset;
        int index=0;
        int size = strlen(path)+1;
        char *check_path = malloc(size); memset(check_path, 0, size); strcpy(check_path, path);
        while(check_path[index]!='\0'){
            int ptr=0; int index_begin=index;
            while((check_path[index]!=' ') && (check_path[index] != '\0')){
                index++;
                ptr++;
            }
            memset(full_path, 0, FILENAME_SIZE);
            strncpy(full_path, check_path+index_begin, ptr);
            if(check_path[index] != '\0') index++;
            printf("full_path is [%s]\n", full_path);

            dest_offset = find_path(cfs_file, current, full_path, false);
            data_type data;
            MDS currentMDS, tempMDS;
            int current_offset;

            lseek(cfs_file, 0, SEEK_SET);
            Superblock *superblock = malloc(sizeof(Superblock));
            read(cfs_file, superblock, sizeof(Superblock));

            lseek(cfs_file, dest_offset, SEEK_SET);
            read(cfs_file, &currentMDS, superblock->metadata_size);
            printf("checkpoint1 %s \n",currentMDS.filename);
            if(currentMDS.type == 2){
                for(int i = 0; i < DATABLOCK_NUM; i++){
                    lseek(cfs_file, currentMDS.data.datablocks[i], SEEK_SET);
                    for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                        read(cfs_file, &data, sizeof(data_type));
                        current_offset = lseek(cfs_file, 0, SEEK_CUR);
                        if(data.active == true){
                            lseek(cfs_file, data.offset, SEEK_SET);
                            read(cfs_file, &tempMDS, superblock->metadata_size);
                            lseek(cfs_file, current_offset, SEEK_SET);
                            if(tempMDS.type != 2){
                                printf("cfs_rm: file %s just deleted\n",data.filename);
                                deleted = true;
                                data.active = false;
                                lseek(cfs_file, -sizeof(data_type), SEEK_CUR);
                                write(cfs_file, &data, sizeof(data_type));
                                delete_from_bitmap(data.offset, cfs_file);
                            }else if(r){
                                char *full_filename=malloc(strlen(full_path)+FILENAME_SIZE);
                                memset(full_filename,0, strlen(full_path)+FILENAME_SIZE);
                                sprintf(full_filename, "%s/%s", full_path, data.filename);
                                cfs_rm(cfs_file, current, full_filename, true);
                                free(full_filename);
                                printf("cfs_rm: folder %s just deleted\n",data.filename);
                                deleted = true;
                                data.active = false;
                                lseek(cfs_file, -sizeof(data_type), SEEK_CUR);
                                write(cfs_file, &data, sizeof(data_type));
                                delete_from_bitmap(data.offset, cfs_file);
                            }

                        }
                    }
                }
            }else{
                printf("cfs_rm: %s: not a directory \n",full_path);
            }
            free(superblock);
        }
        free(check_path);
    }
    else printf("cfs_rm: execute first cfs_workwith\n");
}

void cfs_ln(int cfs_file,  list_node **current, char *source, char *output){
    if(cfs_file>0){
        Superblock *superblock = malloc(sizeof(Superblock));
        int free_offset, source_offset, output_offset;

        char *tmp = strrchr(output, '/'), *filename;
        char *output_path = malloc(strlen(output)+1);
        memset(output_path, 0, strlen(output)+1);
        if(tmp == NULL){
            strcpy(output_path, "./");
            filename = output;
        }
        else{
            strncpy(output_path, output, (int)(tmp-output));
            filename=tmp+1;
        }

        if ((source_offset = find_path(cfs_file, current, source, false))<0)
            printf("cfs_ln: failed to access %s: No such file or directory\n", source);
        if ((output_offset = find_path(cfs_file, current, output_path, false))<0)
            printf("cfs_ln: failed to access %s: No such file or directory\n", output_path);

        if(source>=0 && output>=0){
            data_type data;
            MDS mds, currentMDS, sourceMDS;
            lseek(cfs_file, 0, SEEK_SET);
            read(cfs_file, superblock, sizeof(Superblock));
            bool exists_already = false;
            bool empty_space = false;

            if(find_file(cfs_file, filename)>=0) printf("cfs_ln: '%s' name already exists\n", filename);
            else{
                lseek(cfs_file, source_offset, SEEK_SET);
                read(cfs_file, &sourceMDS, superblock->metadata_size);
                if (sourceMDS.type != 1) printf("cfs_ln: '%s' hard link not allowed for directory\n", filename);
                else{
                    int free_offset = get_space(cfs_file);

                    // Create link
                    superblock->latest_nodeid++;
                    mds.nodeid = superblock->latest_nodeid;
                    mds.offset = free_offset;
                    mds.size = 0;
                    mds.type = 3;
                    mds.parent_nodeid = (*current)->nodeid;
                    mds.parent_offset = (*current)->offset;
                    strcpy(mds.filename, filename);
                    mds.creation_time = time(0); mds.access_time = time(0); mds.modification_time = time(0);
                    mds.data.datablocks[0] = mds.offset + superblock->metadata_size;
                    for(int i = 1; i < DATABLOCK_NUM; i++){
                        mds.data.datablocks[i] = mds.data.datablocks[i-1] + superblock->datablocks_size;
                    }
                    lseek(cfs_file, mds.offset, SEEK_SET);
                    write(cfs_file, &mds, superblock->metadata_size);
                    add_to_bitmap(mds.offset, cfs_file);

                    // Fill empty blocks
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

                    // Fill a data-rec with the info of file-linking
                    lseek(cfs_file, source_offset, SEEK_SET);
                    read(cfs_file, &sourceMDS, superblock->metadata_size);
                    data.nodeid = sourceMDS.nodeid;
                    data.offset = sourceMDS.offset;
                    strcpy(data.filename, sourceMDS.filename);
                    data.active = true;
                    lseek(cfs_file, mds.data.datablocks[0], SEEK_SET);
                    write(cfs_file, &data, sizeof(data_type));

                    // Update parent info
                    lseek(cfs_file, (*current)->offset, SEEK_SET);
                    read(cfs_file, &currentMDS, superblock->metadata_size);
                    for(int i = 0; i < DATABLOCK_NUM; i++) {
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

                }
            }
        }
        free(superblock); free(output_path);
    }
    else printf("cfs_ln: execute first cfs_workwith\n");
}

void cfs_import(int cfs_file,  list_node **current, char *sources, char *directory){
    if(cfs_file>0){
        data_type data;
        MDS allMDS, fileMDS;
        int index=0, size = strlen(sources)+1, file_offset;
        char str_source[FILENAME_SIZE];
        char *check_path = malloc(size); memset(check_path, 0, size); strcpy(check_path, sources);
        while(check_path[index] != '\0'){
            lseek(cfs_file, 0, SEEK_SET);
            Superblock *superblock = malloc(sizeof(Superblock));
            read(cfs_file, superblock, sizeof(Superblock));

            int ptr=0; int index_begin=index;
            while((check_path[index]!=' ') && (check_path[index] != '\0')){
                index++;
                ptr++;
            }
            memset(str_source, 0, FILENAME_SIZE);
            strncpy(str_source, check_path+index_begin, ptr);
            if(check_path[index] != '\0') index++;

            if(strcmp(str_source, directory)!=0){
                DIR *dir_import; struct dirent *dir;
                int file_import, dir_offset=-1;
                if ((dir_import = opendir(str_source)) != NULL){
                    cfs_cd(cfs_file, current, directory);

                    if((find_file(cfs_file, directory))>0){
                        char *name = strrchr(str_source, '/');
                        if(name == NULL) name = str_source;
                        else name++;
                        if(*name!='\0'){

                            dir_offset = cfs_mkdir(cfs_file, name, current);
                            if(dir_offset>0){
                                while((dir = readdir(dir_import)) != NULL) {
                                    if(strcmp(dir->d_name, ".")!=0 && strcmp(dir->d_name, "..")!=0) {

                                        char *source_filename=malloc(strlen(str_source)+FILENAME_SIZE);
                                        memset(source_filename,0, strlen(str_source)+FILENAME_SIZE);
                                        sprintf(source_filename, "%s/%s", str_source, dir->d_name);

                                        cfs_import(cfs_file,  current, source_filename, name);
                                        free(source_filename);
                                    }
                                }
                            }

                        }
                    }
                    cfs_cd(cfs_file, current, "..");
                    closedir(dir_import);
                }
                else if((file_import = open(str_source, O_RDONLY))>0){
                    int size_file = lseek(file_import, 0, SEEK_END);
                    lseek(file_import, 0, SEEK_SET);
                    if(size_file<=BLOCK_SIZE*DATABLOCK_NUM){
                        if((find_file(cfs_file, directory))>0){
                            cfs_cd(cfs_file, current, directory);
                            char *name = strrchr(str_source, '/');
                            if(name == NULL) name = str_source;
                            else name++;
                            if(*name!='\0'){
                                if((file_offset = cfs_touch(cfs_file, true, true, name, current))>0){
                                    lseek(cfs_file, file_offset, SEEK_SET);
                                    read(cfs_file, &fileMDS, superblock->metadata_size);
                                    fileMDS.size = size_file;
                                    lseek(cfs_file, file_offset, SEEK_SET);
                                    write(cfs_file, &fileMDS, superblock->metadata_size);

                                    int ii=0, current_size = BLOCK_SIZE;
                                    while((ii<DATABLOCK_NUM) && (size_file>0)){
                                        if(size_file>0){
                                            lseek(cfs_file, fileMDS.data.datablocks[ii], SEEK_SET);
                                            if(size_file<BLOCK_SIZE) current_size = size_file;
                                            char *buffer=malloc(current_size+1); memset(buffer, 0, current_size+1);
                                            read(file_import, buffer, current_size);
                                            write(cfs_file, buffer, current_size);
                                            free(buffer);
                                            if(size_file>BLOCK_SIZE) size_file -=BLOCK_SIZE;
                                            else size_file=0;
                                        }
                                        ii++;
                                    }

                                }
                            }
                            cfs_cd(cfs_file, current, "..");
                        }
                        else printf("cfs_import: %s: No such file or directory\n", directory);
                    }
                    else printf("cfs_import: no enough space for file\n");
                    close(file_import);
                }
                else printf("cfs_import: failed to access %s: No such file or directory\n", str_source);
            }
            free(superblock);
        }
        free(check_path);
    }
    else printf("cfs_import: execute first cfs_workwith\n");
}

void cfs_export(int cfs_file,  list_node **current, char *sources, char *directory){
    if(cfs_file>0){
        int source_offset;
        data_type data;
        MDS allMDS, fileMDS;
        int size = strlen(sources)+1;
        char *check_path = malloc(size); memset(check_path, 0, size); strcpy(check_path, sources);
        char str_source[FILENAME_SIZE];
        char *pathname = malloc(strlen(directory)+FILENAME_SIZE+5);
        DIR *dir_export, *open_dir;
        FILE *write_fp; int fd;
        struct dirent *dir;
        int index=0;

        if ((dir_export = opendir(directory)) != NULL){
            while(check_path[index] != '\0'){
                int ptr=0; int index_begin=index;
                while((check_path[index]!=' ') && (check_path[index] != '\0')){
                    index++;
                    ptr++;
                }
                memset(str_source, 0, FILENAME_SIZE);
                strncpy(str_source, check_path+index_begin, ptr);
                if(check_path[index] != '\0') index++;

                if(strcmp(str_source, directory)!=0){
                    if((source_offset = find_path(cfs_file, current, str_source, false))<0)
                        printf("cfs_export: failed to access %s: no such file or directory\n", str_source);
                    else{
                        lseek(cfs_file, 0, SEEK_SET);
                        Superblock *superblock = malloc(sizeof(Superblock));
                        read(cfs_file, superblock, sizeof(Superblock));
                        lseek(cfs_file, source_offset, SEEK_SET);
                        read(cfs_file, &fileMDS, superblock->metadata_size);

                        if(fileMDS.type == 1){
                            memset(pathname, 0, strlen(directory)+FILENAME_SIZE+5);
                            sprintf(pathname, "./%s/%s", directory, fileMDS.filename);
                            fd = open(pathname, O_WRONLY | O_CREAT, 0644);
                            if(fd>0){
                                int max_size = fileMDS.size, ii=0, current_size = BLOCK_SIZE;
                                while((ii<DATABLOCK_NUM) && (max_size>0)){
                                    if(max_size>0){
                                        lseek(cfs_file, fileMDS.data.datablocks[ii], SEEK_SET);
                                        if(max_size<BLOCK_SIZE) current_size = max_size;
                                        char *buffer=malloc(current_size+1); memset(buffer, 0, current_size+1);
                                        read(cfs_file, buffer, current_size);
                                        write(fd, buffer, current_size);
                                        free(buffer);
                                        if(max_size>BLOCK_SIZE) max_size -=BLOCK_SIZE;
                                        else max_size=0;
                                    }
                                    ii++;
                                }
                                close(fd);
                            }
                            else printf("cfs_export: open: failed to access %s: no such file or directory\n", pathname);
                        }
                        else if(fileMDS.type == 2){
                            memset(pathname, 0, strlen(directory)+FILENAME_SIZE+5);
                            sprintf(pathname, "%s/%s", directory, fileMDS.filename);
                            if(mkdir(pathname, S_IRWXG)<0) printf("not create dir:[%s]\n", pathname);

                            for(int i = 0; i < DATABLOCK_NUM; i++){
                                lseek(cfs_file, fileMDS.data.datablocks[i], SEEK_SET);
                                for (int j = 0; j < superblock->datablocks_size/(sizeof(data_type)); j++){
                                    read(cfs_file, &data, sizeof(data_type));
                                    int current_pointer = lseek(cfs_file, 0, SEEK_CUR);
                                    if(data.active == true){
                                        lseek(cfs_file, data.offset, SEEK_SET);
                                        read(cfs_file, &allMDS, superblock->metadata_size);
                                        char new_path_name[FILENAME_SIZE*2];
                                        memset(new_path_name, 0, FILENAME_SIZE*2);
                                        sprintf(new_path_name, "%s/%s", pathname, allMDS.filename);
                                        if (allMDS.type == 1){
                                            fd = open(new_path_name, O_WRONLY | O_CREAT, 0644);
                                            if(fd>0){
                                                int max_size = allMDS.size, ii=0;
                                                while((ii<DATABLOCK_NUM) && (max_size>0)){
                                                    if(max_size>0){
                                                        lseek(cfs_file, allMDS.data.datablocks[ii], SEEK_SET);
                                                        char *buffer=malloc(BLOCK_SIZE);
                                                        read(cfs_file, buffer, BLOCK_SIZE);
                                                        write(fd, buffer, BLOCK_SIZE);
                                                        free(buffer);
                                                        if(max_size>BLOCK_SIZE) max_size -=BLOCK_SIZE;
                                                        else max_size=0;
                                                    }
                                                    ii++;
                                                }
                                                close(fd);
                                            }
                                            else printf("cfs_export: open: failed to access %s: no such file or directory\n", pathname);
                                        }
                                        else if (allMDS.type == 2){
                                            if(mkdir(new_path_name, S_IRWXG)<0) printf("not create dir:[%s]\n", new_path_name);
                                        }
                                        lseek(cfs_file, current_pointer, SEEK_SET);
                                    }
                                }
                            }

                        }
                        free(superblock);
                    }
                }

            }
            closedir(dir_export);
        }
        else printf("cfs_export: opendir: failed to access %s: no such file or directory\n", directory);
        free(pathname); free(check_path);
    }
    else printf("cfs_export: execute first cfs_workwith\n");
}
