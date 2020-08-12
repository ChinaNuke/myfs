#ifndef __DATA_BLOCK__H__
#define __DATA_BLOCK__H__

#include "block.h"

/*
 * Data Block
 * 磁盘中位于 Super block 和 Inode Table 之后的数据块
 * 存放文件的实际数据或者间接索引表
 * 通过成组链接法组织管理空闲 block
 */

#define GROUP_SIZE 100

/*
 * 空闲盘块号栈，含1个栈顶指针和最多100个空闲盘块号
 */
typedef struct {
    uint8_t top;
    uint32_t blocks[GROUP_SIZE]; /* 最多100个空闲盘块号 */
} block_stack_t;

/* 数据块区域初始化
 * 将从 data_blocks_start 开始的 data_blocks_count 个盘块初始化为 data blocks
 * 把它们通过成组链接方法链成一条链，并初始化空闲盘块号栈底 stack
 *
 * 成功返回 0，失败返回 BLOCK_ERROR
 */
int data_blocks_init(vdisk_handle_t handle, uint32_t blocksize,
                     uint32_t data_blocks_start, uint32_t data_blocks_count,
                     block_stack_t *stack);

/* 根据成组链接法算法获得一个空闲盘块
 *
 * 返回获得的空闲盘块号
 */
uint32_t data_block_alloc(vdisk_handle_t handle, uint32_t blocksize,
                          block_stack_t *stack);

/* 释放一个盘块使之链入空闲盘块链 */
void data_block_free(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
                     block_stack_t *stack);

/* 从盘块号为 block 的盘块读入空闲盘块号栈到 stack
 * 或把空闲盘块号栈 stack 写入到盘块 block 中
 *
 * 成功返回 0，失败返回 BLOCK_ERROR
 */
int load_group(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               block_stack_t *stack);
int dump_group(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               block_stack_t *stack);

#endif /* __DATA_BLOCK__H__ */
