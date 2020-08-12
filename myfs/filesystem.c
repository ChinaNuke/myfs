#include "filesystem.h"

#include <stdlib.h>
#include <string.h>

/* 除法向上取整 */
#define CEIL_DIVIDE(A, B) (((A) + (B)-1) / (B))

int myfs_format(vdisk_handle_t handle, uint16_t blocksize) {
    /* TODO(peng): 格式化时应建立根目录并在superblock中设置字段指向根目录 */

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

    uint32_t inodes_count = 1 * blocksize * 8;
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

    super_block_t sblock; /* 写在前面因为要用它的 first_group_stack */
    uint32_t data_blocks_start = 2 + inode_blocks;
    uint32_t data_blocks_count = total_blocks - data_blocks_start;
    if (data_blocks_init(handle, blocksize, data_blocks_start,
                         data_blocks_count,
                         &(sblock.first_group_stack)) == BLOCK_ERROR) {
        return MYFS_ERROR;
    }

    /* 初始化 super block */

    sblock.block_size = blocksize;
    sblock.inode_size = INODE_SIZE;
    sblock.blocks_count = data_blocks_count;
    sblock.inodes_count = inodes_count;
    sblock.free_blocks_count = data_blocks_count;
    sblock.free_inodes_count = inodes_count;
    sblock.first_data_block = 1;
    sblock.total_size = total_blocks * blocksize;

    void *buf = calloc(1, blocksize);
    memcpy(buf, &sblock, sizeof(super_block_t));
    if (block_write(handle, blocksize, 0, buf) == BLOCK_ERROR) {
        return MYFS_ERROR;
    }
    free(buf);

    return 0;
}