#ifndef __FILESYSTEM__H__
#define __FILESYSTEM__H__

// #include "block.h"
#include "data_block.h"
#include "inode.h"
#include "super_block.h"

#define MYFS_ERROR -1

int myfs_format(vdisk_handle_t handle, uint16_t blocksize);

int myfs_touch();

int myfs_mkdir();

int myfs_rmdir();

int myfs_link();

int myfs_unlink();

#endif /* __FILESYSTEM__H__ */
