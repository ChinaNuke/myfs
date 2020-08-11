#ifndef __INODE__H__
#define __INODE__H__

#include <stdint.h>
#include <stddef.h>
#include "../virtual_disk.h"
/*
 * inode 为文件和目录的索引节点
 *
 * 一个文件/目录对应一个 inode
 * 一个 inode 可对应多个 block
 *
 * 大小固定为 128bytes（或256bytes）
 */

/* 暂时没有具体考虑每个属性的实际大小，后面再来修改 */
typedef uint32_t BLOCK_ADDR;
#define ADDR_SIZE 4  //一个指向数据块的指针4个字节表示
#define INODE_ERROR -1


typedef struct {
    char mode;  //用户权限
    char owners[2];
    char type; //文件 or 目录
    char time_stamps[3];
    uint64_t size;   //文件的有效长度
    uint16_t block_count;  //已分配给文件的数据块数
    BLOCK_ADDR direct_blocks[12];  //12个直接指向分配给文件的数据块指针
    BLOCK_ADDR single_indirect;
    BLOCK_ADDR double_indirect;
    BLOCK_ADDR triple_indirect;
} inode_t;

typedef uint16_t inode_no_t; //inode号(0-65535) 每个inode 128bytes 总占8M空间
#define INODE_MAX_COUNT (UINT16_MAX - 1)

typedef struct{
    uint16_t is_inquired;  //1 表示已被申请
    inode_t* inode;
}table;

table *inode_table[INODE_MAX_COUNT] = {NULL}; //i节点管理

/*
 * inode:文件的inode
 * no:文件所占的第no块
 * blocksize:规定的块大小
*/
uint32_t block_addr(vdisk_handle_t handle,inode_t* inode,uint16_t no,uint32_t blocksize);


#endif
