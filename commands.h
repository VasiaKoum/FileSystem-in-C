#include <stdbool.h>
#include "structs.h"

void add_dir_to_path(list_node**, unsigned int, unsigned int);
void back_to_path(list_node**, unsigned int);
int edit_commands(char*, int, list_node**);
void cfs_create(char*, int , int , int, int);
int cfs_workwith(char*, list_node**);
void cfs_mkdir(char*, int);
void cfs_touch(bool, bool, char*, int);
void cfs_pwd(int);
