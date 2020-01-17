#include "commands.h"

<<<<<<< Updated upstream
void cfs_create(char *options){

=======
void cfs_create(char* cfs_name, int datablock_size, int filenames_size, int max_file_size, int max_files_in_dirs){
    FILE * cfs_File = NULL;

    cfs_File = fopen(cfs_name, "wb");
    if(cfs_File == NULL){
        printf("Unable to create file.\n");
    }

    Superblock superblock;
    superblock.datablocks-size = blocksize;
    superblock.datablocks-size = blocksize;

    write()

    fclose(cfs_File)

>>>>>>> Stashed changes
}
