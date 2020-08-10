#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <stdint.h>
#include "virtual_disk.h"

/*block模块（文件系统概念）
 *常见的：一个block由连续的八个扇区组成，大小是4k{SECTOR_SIZE（512字节）*8}
 */

#define BLOCK_SIZE SECTOR_SIZE*8  

typedef uint32_t block_handle_t;
#define BLOCK_ERROR -1

/* 读取一个block
 *handle:磁盘设备文件号
 *sectors_per_block：（一般来说是4）
 *no：块号
 *buf:返回块首地址
*/
uint64_t block_read(vdisk_handle_t handle, uint16_t sectors_per_block, block_handle_t no,char* buf)
/*buf:要写入内容的地址 */
uint64_t block_write(vdisk_handle_t handle, uint16_t sectors_per_block, block_handle_t no,const char* buf)

#endif

