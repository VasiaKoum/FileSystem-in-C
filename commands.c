#include "commands.h"

void cfs_create(char* cfs_name, int datablock_size, int filenames_size, int max_file_size, int max_files_in_dirs){
    FILE * cfs_File = NULL;

    cfs_File = fopen(cfs_name, "wb");
    if(cfs_File == NULL){
        printf("Unable to create file.\n");
    }

    Superblock superblock;
    superblock.datablocks_size = datablock_size;
<<<<<<< HEAD
    superblock.metadata_size; = sizeof(MDS);

    write(cfs_File, &superblock, sizeof(superblock));

    fclose(cfs_File)
=======
    superblock.metadata_size; = metadata_size;

    write(cfs_File, &superblock, sizeof(superblock));


    fclose(cfs_File);
>>>>>>> 8a596a8e5c02d85a99965266196c41e9538a4a86

}
