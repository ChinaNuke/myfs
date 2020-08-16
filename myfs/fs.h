#ifndef __FS_H__
#define __FS_H__
#include "filesystem.h"

#define MAX_FS_COUNT 26
#define MAX_BUF_SIZE 1024

myfs_t *filesystems[MAX_FS_COUNT];
char filenames[MAX_FS_COUNT][MAX_BUF_SIZE];

char cur_dir[MAX_BUF_SIZE];
uint8_t cur_fs;

int fs_format(char *filename, uint16_t blocksize);

char fs_mount(char *filename);

int fs_unmount(char c);

char *fs_pwd();

void *fs_disks();

#endif /* __FS_H__*/
