#include "data_block.h"

#include <stdlib.h>
#include <string.h>

int data_blocks_init(vdisk_handle_t handle, uint32_t blocksize,
                     uint32_t data_blocks_start, uint32_t data_blocks_count,
                     block_stack_t *stack) {
    /* full_groups 为刚好有 GROUP_SIZE(100) 个盘块的组数；
     * 最后一组可能不满 GROUP_SIZE 个盘块，这一组含有的盘块数保存到
     * last_group_blocks 中，它的值可能为 0.
     */
    uint32_t full_groups = data_blocks_count / GROUP_SIZE;
    uint8_t last_group_blocks = data_blocks_count % GROUP_SIZE;

    /* 处理空闲盘块号栈和第一个组 */
    stack->top = full_groups > 0 ? (GROUP_SIZE - 1) : (last_group_blocks - 1);
    for (uint8_t j = 0; j <= stack->top; j++) {
        stack->blocks[j] = data_blocks_start + j;
    }

    /* 若 full_groups 为 0，则没有后续处理 */
    if (full_groups > 0) {
        block_stack_t temp_stack;
        /* 处理前面 full_groups - 1 个组，它们各自的下一个组也是 full group */
        temp_stack.top = GROUP_SIZE - 1;
        for (uint32_t i = 0; i < full_groups - 1; i++) {
            /* 当前组的起始块号 */
            uint32_t cur_group_block = data_blocks_start + i * GROUP_SIZE;
            for (uint8_t j = 0; j < GROUP_SIZE; j++) {
                temp_stack.blocks[j] = (cur_group_block + GROUP_SIZE) + j;
            }

            if (dump_group(handle, blocksize, cur_group_block, &temp_stack) !=
                0) {
                return BLOCK_ERROR;
            }
        }

        /* 处理最后一个 full group，它的下一个组可能不满，也可能没有 */
        uint32_t cur_group_block =
            data_blocks_start + (full_groups - 1) * GROUP_SIZE;
        uint8_t j = 0;
        for (; j < last_group_blocks; j++) {
            temp_stack.blocks[j] = (cur_group_block + GROUP_SIZE) + j;
        }
        temp_stack.top = j; /* 注意这里栈顶指针的位置 */
        if (dump_group(handle, blocksize, cur_group_block, &temp_stack) != 0) {
            return BLOCK_ERROR;
        }

        /* TODO(peng): 课本上的描述是最后一组空闲盘块号存放在倒数第二组栈的 1-99
         * 的位置中，0 的位置存放 0 作为结束标志，
         * 而最后一组的首个盘块中存放空闲盘块链的结尾标志，
         * 是否有这样做的必要还有待考究。
         */
    }
    return 0;
}

uint32_t data_block_alloc(vdisk_handle_t handle, uint32_t blocksize,
                          block_stack_t *stack) {
    uint32_t ret = stack->blocks[stack->top];
    /* 如果取出了这组的最后一个空闲盘块号，需要将下一组读入空闲盘块号栈 */
    if (stack->top > 0) {
        stack->top--;
    } else {
        load_group(handle, blocksize, ret, stack);
    }
    return ret;
}

void data_block_free(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
                     block_stack_t *stack) {
    /* 栈已满，把栈的内容写入刚回收的块，此块作为新的栈底 */
    if (stack->top == GROUP_SIZE - 1) {
        dump_group(handle, blocksize, block, stack);
        stack->blocks[0] = block;
        stack->top = 0;
    } else {
        stack->blocks[++(stack->top)] = block;
    }
}

int load_group(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               block_stack_t *stack) {
    void *buf = calloc(1, blocksize);
    int ret = block_read(handle, blocksize, block, buf);
    memcpy(stack, buf, sizeof(block_stack_t));
    free(buf);
    return ret;
}

int dump_group(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               block_stack_t *stack) {
    /* stack不足一个block大小，将其填充为一个block */
    void *buf = calloc(1, blocksize);
    memcpy(buf, stack, sizeof(block_stack_t));
    int ret = block_write(handle, blocksize, block, buf);
    free(buf);
    return ret;
}
