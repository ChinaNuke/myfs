#ifndef __INODE__H__
#define __INODE__H__

#include <stddef.h>
#include <stdint.h>

#include "super_block.h"
#include "virtual_disk.h"

/*
 * inode 为文件和目录的索引节点
 *
 * 一个文件/目录对应一个 inode，一个 inode 可对应多个 block
 * 通过 inode 位图（inode bitmap）来指示对应的 inode 是否空闲
 *
 * 一个 inode 大小固定为 128bytes
 */

// typedef uint32_t BLOCK_ADDR;
#define BLOCK_ADDR_LEN 4  //一个指向数据块的指针4个字节表示
#define INODE_ERROR -1

#define INODE_SIZE 128

typedef struct {
    uint16_t mode; /* 文件类型和访问权限 */
    uint16_t uid;  /* 拥有者的标识符 */
    uint32_t size; /* 文件大小（字节）,单个文件限制为最大4GB */
    uint32_t direct_blocks[12]; /* 12个直接指向分配给文件的数据块指针 */
    uint32_t single_indirect;
    uint32_t double_indirect;
    uint32_t triple_indirect;
    uint32_t atime;       /* 访问时间 access time */
    uint32_t ctime;       /* 创建时间 creation time */
    uint32_t mtime;       /* 修改时间 modification time */
    uint16_t links_count; /* 硬链接计数器 */
    uint32_t blocks;      /* 文件的数据块数 */
    uint32_t block;       /* 指向数据块的指针 */
} inode_t;

typedef uint16_t inode_no_t;  // inode号(0-65535) 每个inode 128bytes 总占8M空间
#define INODE_MAX_COUNT (UINT16_MAX - 1)

/* 根据文件的第 i 个块找出这个块在磁盘上的块号 */
int64_t locate_block(vdisk_handle_t handle, uint16_t blocksize, inode_t* inode,
                     uint32_t i);

uint16_t inode_alloc(super_block_t* sb, uint8_t* bitmap);
void inode_free(super_block_t* sb, uint8_t* bitmap, uint16_t inode);

inode_t* load_inode(vdisk_handle_t handle, uint16_t blocksize, uint16_t inode);

int dump_inode(vdisk_handle_t handle, uint16_t blocksize, uint16_t inode,
               inode_t* one_inode);

int inode_append_block(vdisk_handle_t handle, uint16_t blocksize,
                       inode_t* one_inode);

int inode_reduce_block(vdisk_handle_t handle);

#endif
