#include "inode.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"

int64_t locate_block(vdisk_handle_t handle, uint16_t blocksize, inode_t* inode,
                     uint32_t i) {
    const uint16_t addrs_per_block = blocksize / BLOCK_ADDR_LEN;
    const uint32_t direct_max = 12;
    const uint32_t single_max = direct_max + addrs_per_block;
    const uint32_t double_max = single_max + addrs_per_block * addrs_per_block;
    const uint32_t triple_max =
        double_max + addrs_per_block * addrs_per_block * addrs_per_block;

    // 直接寻址
    if (i < direct_max) {
        return inode->direct_blocks[i];
    }

    // 一次间址
    if (i >= direct_max && i < single_max) {
        uint32_t* buf = (uint32_t*)malloc(addrs_per_block * sizeof(uint32_t));
        if (block_read(handle, blocksize, inode->single_indirect, buf) ==
            BLOCK_ERROR) {
            free(buf);
            return INODE_ERROR;
        }
        uint32_t ret = buf[i - direct_max];
        free(buf);
        return ret;
    }

    // 两次间址
    if (i >= single_max && i < double_max) {
        uint32_t buf[addrs_per_block];

        // 先读二次间地址的直接地址块
        if (block_read(handle, blocksize, inode->double_indirect, buf) ==
            BLOCK_ERROR) {
            return INODE_ERROR;
        }

        // 再读取直接地址块的第site个地址指向的block
        i = i - single_max;
        uint32_t j = buf[i / addrs_per_block];
        if (block_read(handle, blocksize, j, buf) == BLOCK_ERROR) {
            return INODE_ERROR;
        }
        return buf[i % addrs_per_block];
    }

    // 三次间址
    if (i >= double_max && i < triple_max) {
        uint32_t buf[addrs_per_block];

        //先读三次间地址的直接地址块
        if (block_read(handle, blocksize, inode->triple_indirect, buf) ==
            BLOCK_ERROR) {
            return INODE_ERROR;
        }

        //再读取直接地址块的第site个地址指向的block
        i = i - double_max;
        uint32_t j = buf[i / (addrs_per_block * addrs_per_block)];
        if (block_read(handle, blocksize, j, buf) == BLOCK_ERROR) {
            return INODE_ERROR;
        }

        //再读取间接地址块的第n个地址指向的block
        i = i % addrs_per_block;
        uint32_t k = buf[i / (addrs_per_block)];
        if (block_read(handle, blocksize, k, buf) == BLOCK_ERROR) {
            return INODE_ERROR;
        }
        return buf[i % addrs_per_block];
    }
    assert(0);
    return INODE_ERROR;
}

int32_t inode_alloc(super_block_t* sb, uint8_t* bitmap) {
    for (uint16_t i = 0; i < sb->inodes_count / 8;
         i++) {                  /* 遍历block上每个字节 */
        if (bitmap[i] != 0xFF) { /* 有字节含有 0 位（即有空闲） */
            /* 遍历每一位找到首个为 0 的位，将其置 1（分配出去）
             * 计算并返回该位对应的 inode 号 */
            for (uint8_t j = 0; j < 8; j++) {
                if ((bitmap[i] & (1 << j)) == 0) {
                    bitmap[i] |= (1 << j);
                    sb->free_inodes_count--;
                    return 8 * i + j;
                }
            }
        }
    }
    return INODE_ERROR;
}

void inode_free(super_block_t* sb, uint8_t* bitmap, uint16_t inode) {
    uint16_t i = inode / 8; /* inode 所对应 bitmap 中哪个字节 */
    uint8_t j = inode % 8;  /* 字节中的哪一位 */
    bitmap[i] &= ~(1 << j); /* 将对应的位清零（回收inode） */
    sb->free_inodes_count++;
    /* TODO(peng): 或许后面还要进行一些回收的其他操作 */
}

inode_t* load_inode(vdisk_handle_t handle, uint16_t blocksize, uint16_t inode) {
    uint16_t inodes_per_block = blocksize / INODE_SIZE;
    uint32_t at_block = inode / inodes_per_block + 2; /* 确定inode所在的块号 */
    uint16_t offset = inode % inodes_per_block; /* inode在块内的偏移 */

    inode_t* one_inode = malloc(sizeof(inode_t));
    void* buf = malloc(blocksize * sizeof(char));

    block_read(handle, blocksize, at_block, buf);
    memcpy(one_inode, buf + offset * INODE_SIZE, sizeof(inode_t));

    free(buf);
    return one_inode;
}

int dump_inode(vdisk_handle_t handle, uint16_t blocksize, uint16_t inode,
               inode_t* one_inode) {
    uint16_t inodes_per_block = blocksize / INODE_SIZE;
    uint32_t at_block = inode / inodes_per_block + 2; /* 确定inode所在的块号 */
    uint16_t offset = inode % inodes_per_block; /* inode在块内的偏移 */

    void* buf = malloc(blocksize * sizeof(char));
    block_read(handle, blocksize, at_block, buf);
    memcpy(buf + offset * INODE_SIZE, one_inode, sizeof(inode_t));
    block_write(handle, blocksize, at_block, buf);

    free(buf);
    // free(one_inode);
    return 0;
}

int inode_append_block(vdisk_handle_t handle, super_block_t* sb,
                       inode_t* inode_struct) {
    uint16_t blocksize = sb->block_size;
    const uint16_t addrs_per_block = blocksize / BLOCK_ADDR_LEN;
    const uint32_t direct_max = 12;
    const uint32_t single_max = direct_max + addrs_per_block;
    const uint32_t double_max = single_max + addrs_per_block * addrs_per_block;
    const uint32_t triple_max =
        double_max + addrs_per_block * addrs_per_block * addrs_per_block;

    if (inode_struct->blocks >= triple_max) {
        return -1;
    }

    uint32_t new_block = data_block_alloc(handle, blocksize, &sb->group_stack);
    uint32_t blocks_count = inode_struct->blocks;
    if (blocks_count < direct_max) {
        inode_struct->direct_blocks[blocks_count] = new_block;
    } else if (blocks_count >= direct_max && blocks_count < single_max) {
        if (blocks_count == direct_max) {
            uint32_t new_addr_block =
                data_block_alloc(handle, blocksize, &sb->group_stack);
            inode_struct->single_indirect = new_addr_block;
        }
        uint32_t* single_indirect_blk = malloc(blocksize);
        block_read(handle, blocksize, inode_struct->single_indirect,
                   single_indirect_blk);
        single_indirect_blk[blocks_count - direct_max] = new_block;
        block_write(handle, blocksize, inode_struct->single_indirect,
                    single_indirect_blk);
        free(single_indirect_blk);
    }
    /* TODO: 目前只考虑了直接寻址和一次间址，后面再完善二次和三次间址 */
    inode_struct->blocks++;
    return new_block;
}
