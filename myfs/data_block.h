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
    uint32_t blocks[100]; /* 最多100个空闲盘块号 */
} block_stack_t;

int data_blocks_init(vdisk_handle_t handle, uint32_t blocksize,
                     uint32_t data_blocks_start, uint32_t data_blocks_count,
                     block_stack_t *stack);
uint32_t data_block_alloc(vdisk_handle_t handle, uint32_t blocksize,
                          block_stack_t *stack);
void data_block_free(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
                     block_stack_t *stack);

int load_group(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               block_stack_t *stack);
int dump_group(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               block_stack_t *stack);
#endif /* __DATA_BLOCK__H__ */
