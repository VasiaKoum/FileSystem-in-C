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
<<<<<<< HEAD
		unsigned int datablocks_size;
		unsigned int metadata_size;
	// thesi domis metadata katalogou, path?
=======
	unsigned int datablocks_size;
	unsigned int metadata_size;
	char path_root[100];
>>>>>>> 8a596a8e5c02d85a99965266196c41e9538a4a86
} Superblock;
