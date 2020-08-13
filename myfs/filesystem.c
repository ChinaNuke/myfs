#include "filesystem.h"

#include <stdlib.h>
#include <string.h>

/* 除法向上取整 */
#define CEIL_DIVIDE(A, B) (((A) + (B)-1) / (B))

int myfs_format(vdisk_handle_t handle, uint16_t blocksize) {
    /*
     * blocksize 一般为 1KB/2KB/4KB
     *
     * 0 号块：super block
     * 1 号块：inode bitmap
     */

    uint32_t device_size = vdisk_get_size(handle); /* 设备大小（字节） */
    uint32_t total_blocks = device_size / blocksize; /* 多出来的不使用 */

    /* 初始化 inode bitmap 区和 inode table 区
     * 默认 inode bitmap 固定占一个 block，即紧随 superblock 之后的 1 号块
     * 在 blocksize 为 4096 的情况下，最多支持 32768 个文件
     */

    void *empty_block = calloc(1, blocksize);
    block_write(handle, blocksize, 1, empty_block); /* 写入 bitmap */

    uint16_t inodes_count = 1 * blocksize * 8;
    uint32_t inodes_size = inodes_count * INODE_SIZE;
    uint32_t inode_blocks = CEIL_DIVIDE(inodes_size, blocksize);

    /* 写入 inode table */
    for (uint32_t i = 0; i < inode_blocks; i++) {
        if (block_write(handle, blocksize, 2 + i, empty_block) == BLOCK_ERROR) {
            return MYFS_ERROR;
        }
    }
    free(empty_block);

    /* 初始化 data block 区 */

    super_block_t sb; /* 写在前面因为要用它的 first_group_stack */
    uint32_t data_blocks_start = 2 + inode_blocks;
    uint32_t data_blocks_count = total_blocks - data_blocks_start;
    if (data_blocks_init(handle, blocksize, data_blocks_start,
                         data_blocks_count,
                         &(sb.first_group_stack)) == BLOCK_ERROR) {
        return MYFS_ERROR;
    }

    /* 建立根目录 */
    uint32_t root_block =
        data_block_alloc(handle, blocksize, &(sb.first_group_stack));

    /* 初始化 super block */

    sb.block_size = blocksize;
    sb.inode_size = INODE_SIZE;
    sb.blocks_count = data_blocks_count;
    sb.inodes_count = inodes_count;
    sb.free_blocks_count = data_blocks_count;
    sb.free_inodes_count = inodes_count;
    // sb.first_data_block = 1;
    sb.total_size = total_blocks * blocksize;

    void *buf = calloc(1, blocksize);
    memcpy(buf, &sb, sizeof(super_block_t));
    if (block_write(handle, blocksize, 0, buf) == BLOCK_ERROR) {
        return MYFS_ERROR;
    }
    free(buf);

    return 0;
}

int myfs_mount(vdisk_handle_t handle, myfs_t **fs) {
    myfs_t *filesystem = (myfs_t *)malloc(sizeof(myfs_t));
    super_block_t *sblock = (super_block_t *)malloc(sizeof(super_block_t));

    /* 默认 super block 大小不会超过1K，所以在不知道 blocksize 的情况下按 1K
     * 读出 super block */
    void *buf = calloc(1, BLK_SIZE_1K);
    if (block_read(handle, BLK_SIZE_1K, 0, buf) != 0) {
        return MYFS_ERROR;
    }
    memcpy(sblock, buf, sizeof(super_block_t));
    free(buf);

    filesystem->sb = sblock;
    filesystem->disk_handle = handle;
    *fs = filesystem;
    return 0;
}

int myfs_unmount(myfs_t *fs) {
    free(fs->sb);
    free(fs);
    return 0;
}
