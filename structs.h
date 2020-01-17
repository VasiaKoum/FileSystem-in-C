#include <time.h>
#define DATABLOCK_NUM 512

typedef struct{
	unsigned int nodeid;
	char *filename;
	unsigned int size;
	unsigned int type;
	unsigned int parent-nodeid;
	time_t creation_time;
	time_t access_time;
	time_t modification_time;
	Datastream data;
} MDS;

typedef struct{
	unsigned int datablocks[DATABLOCK_NUM];
} Datastream;

typedef struct{
	unsigned int datablocks-size;
	unsigned int metadata-size;
	// thesi domis arxikou katalogou, path?
} Superblock;
