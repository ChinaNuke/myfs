#ifndef __FS_H__
#define __FS_H__
#include "dir_entry.h"
#include "filesystem.h"

#define MAX_FS_COUNT 26
#define MAX_BUF_SIZE 1024

myfs_t *filesystems[MAX_FS_COUNT];
char filenames[MAX_FS_COUNT][MAX_BUF_SIZE];

char cur_dir[MAX_BUF_SIZE];
uint8_t cur_fs;
dir_entry_t *cur_dentry;

int fs_format(char *filename, uint16_t blocksize);

char fs_mount(char *filename);

int fs_unmount(char c);

char *fs_pwd();

void fs_df();

int fs_chdisk(char c);

int fs_mkdir(char *path);

int fs_ls();

int fs_touch(char *path);

int fs_rm(char *path);

int fs_cd(char *path);

int fs_stat(char *path);

int fs_link(char *link, char *path);

int fs_chmod(char *path, uint16_t perm);

#endif /* __FS_H__*/
