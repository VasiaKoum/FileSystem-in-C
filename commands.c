#include "commands.h"

void cfs_create(char* cfs_name, int datablock_size, int filenames_size, int max_file_size, int max_files_in_dirs){
    FILE * cfs_File = NULL;
    int byteswritten = 0;

    cfs_File = fopen(cfs_name, "wb");
    if(cfs_File == NULL){
        printf("Unable to create file.\n");
    }

    Superblock superblock;
    superblock.datablocks_size = datablock_size;
    superblock.metadata_size; = sizeof(MDS);
    superblock.

    lseek()
    byteswritten = write(cfs_File, &superblock, sizeof(superblock));
    if(byteswritten != sizeof(superblock)){
        printf("superblock not written\n");
    }


    fclose(cfs_File)

}
