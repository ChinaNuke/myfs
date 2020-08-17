#ifndef __FILESYSTEM__H__
#define __FILESYSTEM__H__

#include "data_block.h"
#include "dir_entry.h"
#include "inode.h"
#include "super_block.h"

#define MYFS_ERROR -1

typedef struct {
    vdisk_handle_t disk_handle;
    super_block_t *sb;
    uint8_t *bitmap;
} myfs_t;

// uint16_t cur_fs = -1;

int myfs_format(vdisk_handle_t handle, uint16_t blocksize);

int myfs_mount(vdisk_handle_t handle, myfs_t *fs);

int myfs_unmount(myfs_t *fs);

int myfs_ls(myfs_t *fs, dir_entry_t *cur_dir);

int myfs_touch();

int myfs_mkdir(myfs_t *fs, dir_entry_t *cur_dir, char *path);

int myfs_rm(myfs_t *fs, dir_entry_t *cur_dir, char *path);

int myfs_link(myfs_t *fs, char *link, dir_entry_t *cur_dir, char *targetname);

int myfs_unlink();

dir_entry_t *myfs_search_dentry(myfs_t *fs, dir_entry_t *parent,
                                const char *dentryname);

int myfs_cd(char *path);

int myfs_stat(myfs_t *fs, dir_entry_t *cur_dir, char *path);

#endif /* __FILESYSTEM__H__ */
