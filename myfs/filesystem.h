#ifndef __FILESYSTEM__H__
#define __FILESYSTEM__H__

#include "data_block.h"
#include "inode.h"
#include "super_block.h"

#define MYFS_ERROR -1

enum blk_size { BLK_SIZE_1K = 1024, BLK_SIZE_2K = 2048, BLK_SIZE_4K = 4096 };

typedef struct {
    vdisk_handle_t disk_handle;
    super_block_t *sb;
    uint8_t *bitmap;
} myfs_t;

int myfs_format(vdisk_handle_t handle, uint16_t blocksize);

int myfs_mount(vdisk_handle_t handle, myfs_t **fs);

int myfs_unmount(myfs_t *fs);

int myfs_touch();

int myfs_mkdir(myfs_t *fs, char *path);

int myfs_rmdir();

int myfs_chdir();

int myfs_link();

int myfs_unlink();

#endif /* __FILESYSTEM__H__ */
