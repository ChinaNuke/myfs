#ifndef __INODE__H__
#define __INODE__H__

#include <stdint.h>

/*
 * inode 为文件和目录的索引节点
 *
 * 大小固定为 128bytes（或256bytes）
 */

/* 暂时没有具体考虑每个属性的实际大小，后面再来修改 */
typedef struct {
    char mode;
    char owners[2];
    char time_stamps[3];
    uint16_t size;
    uint16_t block_count;
    uint16_t direct_blocks[10];
    uint16_t single_indirect;
    uint16_t double_indirect;
    uint16_t triple_indirect;
} inode_t;

#endif
