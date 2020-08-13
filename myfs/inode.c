#include "inode.h"

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
        uint32_t buf[addrs_per_block];
        if (block_read(handle, inode->single_indirect, blocksize, buf) ==
            BLOCK_ERROR) {
            return INODE_ERROR;
        }
        return buf[i - direct_max];
    }

    // 两次间址
    if (i >= single_max && i < double_max) {
        uint32_t buf[addrs_per_block];

        // 先读二次间地址的直接地址块
        if (block_read(handle, inode->double_indirect, blocksize, buf) ==
            BLOCK_ERROR) {
            return INODE_ERROR;
        }

        // 再读取直接地址块的第site个地址指向的block
        i = i - single_max;
        uint32_t j = buf[i / addrs_per_block];
        if (block_read(handle, j, blocksize, buf) == BLOCK_ERROR) {
            return INODE_ERROR;
        }
        return buf[i % addrs_per_block];
    }

    // 三次间址
    if (i >= double_max && i < triple_max) {
        uint32_t buf[addrs_per_block];

        //先读三次间地址的直接地址块
        if (block_read(handle, inode->triple_indirect, blocksize, buf) ==
            BLOCK_ERROR) {
            return INODE_ERROR;
        }

        //再读取直接地址块的第site个地址指向的block
        i = i - double_max;
        uint32_t j = buf[i / (addrs_per_block * addrs_per_block)];
        if (block_read(handle, j, blocksize, buf) == BLOCK_ERROR) {
            return INODE_ERROR;
        }

        //再读取间接地址块的第n个地址指向的block
        i = i % addrs_per_block;
        uint32_t k = buf[i / (addrs_per_block)];
        if (block_read(handle, k, blocksize, buf) == BLOCK_ERROR) {
            return INODE_ERROR;
        }
        return buf[i % addrs_per_block];
    }
    return INODE_ERROR;
}

uint16_t inode_alloc(vdisk_handle_t handle, uint16_t blocksize,
                     uint8_t* bitmap) {
    /* TODO(peng): 这里实际上可以利用superblock上的信息进行优化，
     * 不必要遍历整个block */

    for (uint16_t i = 0; i < blocksize; i++) { /* 遍历block上每个字节 */
        if (bitmap[i] != 0xFF) { /* 有字节含有 0 位（即有空闲） */
            /* 遍历每一位找到首个为 0 的位，将其置 1（分配出去）
             * 计算并返回该位对应的 inode 号 */
            for (uint8_t j = 0; j < 8; j++) {
                if ((bitmap[i] & (1 << j)) == 0) {
                    bitmap[i] |= (1 << j);
                    return 8 * i + j;
                }
            }
        }
    }
    return INODE_ERROR;
}

void inode_free(vdisk_handle_t handle, uint16_t inode, char* bitmap) {
    uint16_t i = inode / 8; /* inode 所对应 bitmap 中哪个字节 */
    uint8_t j = inode % 8;  /* 字节中的哪一位 */
    bitmap[i] &= ~(1 << j); /* 将对应的位清零（回收inode） */

    /* TODO(peng): 或许后面还要进行一些回收的其他操作 */
}
