#include <stdbool.h>
#include "structs.h"

void add_dir_to_path(list_node**, unsigned int, unsigned int, char*);
void back_to_path(list_node**, unsigned int, int);
void print_current_path(list_node**);
int edit_commands(char*, int, list_node**);
void cfs_create(char*, int , int , int, int);
int cfs_workwith(char*, list_node**);
void cfs_mkdir(char*, int);
void cfs_touch(bool, bool, char*, int);
void cfs_pwd(int, list_node**);
void cfs_cd(int, list_node**, char*);
int find_path(int, list_node**, char *);
void add_to_bitmap(int, int);
int get_space(int);
void delete_from_bitmap(int, int);
