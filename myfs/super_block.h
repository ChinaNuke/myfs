#ifndef __SUPER_BLOCK__H__
#define __SUPER_BLOCK__H__

#include <stdint.h>

/*
 * Superblock 是文件系统最基本的元数据，它记录此 filesystem 的整体信息，
 * 包括inode/block的总量、使用量、剩余量，以及档案系统的格式与相关信息等
 * Superblock 的大小一般为固定的 1024bytes
 */

/* 暂时没有具体考虑每个属性的实际大小，统一写为 uint16，后面再来修改 */
typedef struct {
    uint16_t block_size; /* 块的字节数，一般为1K，2K，4K */
    uint16_t inode_size;
    uint16_t inode_count;
    uint16_t block_count;
    uint16_t free_inodes;
    uint16_t free_blocks;
    uint16_t total_size;
} super_block_t;

#endif
