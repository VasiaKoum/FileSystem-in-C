#include <time.h>
#define DATABLOCK_NUM 512
#define FILENAME_SIZE 200

typedef struct{
	unsigned int datablocks[DATABLOCK_NUM];
} Datastream;

typedef struct{
	unsigned int nodeid;
	unsigned int offset;
	char *filename;
	unsigned int size;
	unsigned int type;
	unsigned int parent_nodeid;
	time_t creation_time;
	time_t access_time;
	time_t modification_time;
	Datastream data;
} MDS;

typedef struct{
	unsigned int datablocks_size;
	unsigned int metadata_size;
	char path_root[FILENAME_SIZE];
} Superblock;

typedef struct{
	char *array;
} Bitmap;
