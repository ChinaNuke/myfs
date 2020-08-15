#ifndef __BLOCK__H__
#define __BLOCK__H__

#include <stdint.h>

#include "virtual_disk.h"

/*
 * Block
 * 一个 Block 可由一个或多个扇区组成
 * 文件系统以 block 为单位对磁盘进行读写，一次读写一个块
 *
 * block 大小在文件系统格式化时确定，可以为 1K， 2K， 4K 三种
 */

#define BLOCK_ERROR -1

enum blk_size { BLK_SIZE_1K = 1024, BLK_SIZE_2K = 2048, BLK_SIZE_4K = 4096 };

/*
 * 读取一个block
 * handle: 磁盘设备文件号
 * block: 起始块号
 * blocksize: 一个块的大小（字节）
 * buf: 输出缓冲区
 * 成功返回0，失败返回BLOCK_ERROR
 */
int block_read(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               void* buf);

/*
 * 读取一个block
 * handle: 磁盘设备文件号
 * block: 起始块号
 * blocksize: 一个块的大小（字节）
 * buf: 输出缓冲区
 * 成功返回0，失败返回BLOCK_ERROR
 */
int block_write(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
                const void* buf);

#endif /* __BLOCK__H__ */
