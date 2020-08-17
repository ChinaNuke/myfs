#ifndef __SUPER_BLOCK__H__
#define __SUPER_BLOCK__H__

#include <stdint.h>

#include "data_block.h"

/*
 * Superblock 是文件系统最基本的元数据，它记录此 filesystem 的整体信息，
 * 包括 inode 和 block 的总量、使用量、剩余量，以及档案系统的格式与相关信息等
 * Superblock 的大小一般为固定的 1024bytes
 * 默认它单独占用一个 block
 */

typedef struct {
    uint16_t block_size; /* 块的字节数，一般为1K，2K，4K */
    uint16_t inode_size; /* inode 大小，128Byte，目前情况可以考虑不要这个字段 */
    uint32_t blocks_count; /* data block 数 */
    uint16_t inodes_count;
    uint32_t free_blocks_count; /* 空闲 data block 数 */
    uint16_t free_inodes_count;
    uint64_t total_size;       /* 文件系统大小（字节） */
    block_stack_t group_stack; /* 第一个空闲盘块号栈 */
    //    dir_entry_t *root_dentry;
} super_block_t;

int load_super_block(vdisk_handle_t handle, super_block_t *sb);
int dump_super_block(vdisk_handle_t handle, super_block_t *sb);

#endif
