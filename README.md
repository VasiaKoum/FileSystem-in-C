# FileSystem
#### Assignment 4 for lesson: [`K22 Operating System`](http://cgi.di.uoa.gr/~ad/k22/) (DI University of Athens)
Task
------------
[`Project 4 for Winter Semester 2019-2020`](http://cgi.di.uoa.gr/~ad/k22/OS-F19-Prj4.pdf)

Contributors
------------
* [Vasiliki Koumarela](https://github.com/VasiaKoum/ "Vasiliki Koumarela")
* [Charalampos Katimertzis](https://github.com/chariskms/ "Charalampos Katimertzis")

Implementation
--------------
This filesystem contains files, dirs & hard links. The structure is:
* __Superblock__: datablock_size, metadata_size, root_metadata_offset, latest_nodeid
* __Bitmap__: map the allocated space
* __MDSs__(MetaData Structure) for all files: nodeid, offset, filename, size, type, parent_nodeid, parent_offset, creation_time, access_time, modification_time and an array(datablocks) with the offsets for saved data e.g. saved data for dirs are structs(data_type) for files/dirs/links that are contained in dir.

File organization looks like this:
![fs_img](https://user-images.githubusercontent.com/26937033/78558519-64dee200-781b-11ea-8edc-1c3e88af5b66.JPG)

*The options for commands are combined like -l -a (NOT -la)!*

Testing files are included for cfs_import command.

Run commands
------------
Compile and execute:
```
make
make run
```
Clean:
```
make clean
```

First, to create a file .cfs(FILE the name you want without .cfs) run:
```
cfs_create <OPTIONS> <FILE>
<OPTIONS>:     -bs <BLOCK_SIZE> (default 512)
               -fns <FILENAME_SIZE> (default 200)
               -cfs <MAX_FILE_SIZE> (default 3000)
               -mdfn <MAX_DIRECTORY_FILE_NUMBER> (default 10)
```
To work with a cfs file(e.g. filename.cfs) run:
```
cfs_workwith <FILE>
```
Then you can run the follow commands:
```
1.  cfs_mkdir <DIRECTORIES>
2.  cfs_touch <OPTIONS> <FILES>
    <OPTIONS>: -a (update access time)
               -m (update modification time)
3.  cfs_pwd
4.  cfs_cd <PATH>
5.  cfs_ls <OPTIONS> <FILES>
    <OPTIONS>: -a (do not ignore entries starting with .)
               -r (list subdirectories recursively)
               -l (use a long listing format)
               -u (do not sort; list entries in directory order)
               -d (list directory entries instead of contents)
               -h (list links entries)
6.  cfs_cat <SOURCE FILES> -o <OUTPUT FILE>
7.  cfs_ln <SOURCE FILES> <OUTPUT FILE>
8.  cfs_mv <OPTIONS> <SOURCE> <DESTINATION> | <OPTIONS> <SOURCES> ... <DIRECTORY>
9.  cfs_rm <OPTIONS> <DESTINATIONS>
10. cfs_import <SOURCES> ... <DIRECTORY> (import dirs/files-> SOURCES local path of your system, DIRECTORY in cfs file)
11. cfs_export <SOURCES> ... <DIRECTORY> (export dirs/files-> SOURCES in cfs file, DIRECTORY local path of your system)
12. exit (to exit from program)
```
