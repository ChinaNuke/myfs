#ifndef __DATA_BLOCK__H__
#define __DATA_BLOCK__H__

/*
 * Data Block
 * 磁盘中位于 Super block 和 Inode Table 之后的数据块
 * 存放文件的实际数据或者间接索引表
 * 通过成组链接法组织管理空闲 block
 */

typedef struct {
} data_block_t;

#endif /* __DATA_BLOCK__H__ */
