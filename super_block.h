#ifndef __SUPER_BLOCK__H__
#define __SUPER_BLOCK__H__

#include <stdint.h>

/*
 * Superblock 是文件系统最基本的元数据
 * 它定义了文件系统的类似、大小、状态，和其他元数据结构的信息（元数据的元数据）
 *
 */

typedef struct {
    uint16_t blocksize; /* 块的字节数 */

} super_block_t;

#endif
