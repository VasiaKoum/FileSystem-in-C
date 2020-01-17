#include "commands.h"

void cfs_create(char* cfs_name, int datablock_size, int filenames_size, int max_file_size, int max_files_in_dirs){
    FILE * cfs_File = NULL;

    cfs_File = fopen(cfs_name, "wb");
    if(cfs_File == NULL){
        printf("Unable to create file.\n");
    }

    Superblock superblock;
    superblock.datablocks_size = datablock_size;
    superblock.metadata_size; = datablock_size;

    int write(cfs_File, &superblock, sizeof(superblock));

    fclose(cfs_File)

}
