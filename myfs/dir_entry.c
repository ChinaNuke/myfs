#include "dir_entry.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"
#include "data_block.h"

// dir_entry_t* block_to_dir(vdisk_handle_t handle, uint32_t block,
//                          uint32_t blocksize) {
//    void* buf = calloc(1, blocksize);
//    int res = block_read(handle, blocksize, block, buf);
//    if (res == -1) return NULL;

//    int dir_num = blocksize / REC_LEN; /*一个块的目录项数目 */
//    /*将一个目录blcok拆分成多个目录项 */
//    dir_entry_t* dir[dir_num];
//    for (int i = 0; i < dir_num; i++) {
//        memcpy(dir[i], buf + (i * REC_LEN), sizeof(dir_entry_t));
//    }
//    free(buf);
//    return *dir;
//}

// int search_inode_addr(char* name, vdisk_handle_t handle, uint32_t block,
//                      uint32_t blocksize, uint8_t* site) {
//    int dir_num = blocksize / REC_LEN; /*一个块的目录项数目 */
//    dir_entry_t* dir = block_to_dir(handle, block, blocksize);
//    /*通过文件名查找indoe号 */

//    for (uint8_t i = 0; i < dir_num; i++) {
//        if (dir[i].inode_addr ==
//            0) {
//            /*blcok的一个目录项大小的空间可用，应该是NULL还是inode_addr为0
//                    删除操作是将inode_addr置0*/
//            if (*site == 0) *site = i;  //地址最小的blcok没存满
//            i这个位置是空的
//        }
//        if (strcmp(dir[i].name, name) == 0) {
//            return dir[i].inode_addr;
//        }
//    }
//    /* 没找到返回-1*/
//    return DIR_ENTRY_ERROR;
//}

// uint8_t name2type_no(char* name) {}

// int add_document(char* name, vdisk_handle_t handle, uint32_t blocksize,
//                 uint16_t inode_id) {
//    /*输入indoe号，从磁盘读出inode,从indoe的地址读出blcok，转成目录项数组，添加目录项，（写回）
//     */

//    /*
//     *inode.h规定inode是128字节，一个block有 blocksize/128 个inode
//     *已知inode_id,读取的block_id是 inode_id/(blocksize/128)
//     */

//    uint32_t block_id = inode_id / (blocksize / 128);

//    void* buf = calloc(1, blocksize);
//    int res = block_read(handle, blocksize, block_id, buf);

//    if (res == -1) return DIR_ENTRY_ERROR;
//    /*取出一个blcok中的指定的inode */
//    inode_t* inode;
//    memcpy(inode, buf + (inode_id % (blocksize / 128) * 128),
//    sizeof(inode_t));

//    free(buf);
//    /*取inode地址指向的block(目录项集合)
//     *可能分布在几个块（当前文件夹中有很多文件）
//     *就得找到一个有空位置的block

//    */
//    /*0号blaock和block的0号位置（记录当前目录）不会做给一般文件的目录项，这里
//     *这里将判重名文件之后添加记录项的blcok号，和block
//     *里面的第几个位置在判重过程中被改写
//     */
//    uint32_t add_document_block = 0;
//    uint8_t site;

//    for (uint32_t i = 0; i < inode->blocks;
//         i++) { /*遍历indoe说明的数据块个数 */

//        uint32_t block_id =
//            locate_block(handle, inode, i,
//                         blocksize);  //找到block_id,block_addr在inode.h文件

//        if (search_inode_addr(name, handle, block_id, blocksize, &site) !=
//            -1) {  //使用文件名找到了inode_id，添加的文件名重复了
//            if (site != 0) {                 /*该block有空闲位置 */
//                if (add_document_block == 0) /*保留最早一次修改 */
//                    add_document_block = block_id;
//            }
//            // printf("%s","文件已存在！");
//            return DIR_ENTRY_ERROR;
//        }
//    }
//    /*遍历完所有该inode的所有block没找到该文件名对应的记录项*/
//    dir_entry_t* dir = block_to_dir(handle, add_document_block, blocksize);
//    strcpy(dir[site].name, name);
//    dir[site].file_type_id = name2type_no(name);
//    /*差一个 为该目录分配inode号（indoe管理 ） */
//    // dir[site].inode_addr = 【inode号】

//    /*写回被修改的block */
//    void* buf_w = calloc(1, blocksize);
//    int dir_num = blocksize / REC_LEN; /*一个块的目录项数目 */
//    for (int i = 0; i < dir_num; i++) {
//        memcpy(buf + (i * REC_LEN), &dir[i], sizeof(dir_entry_t));
//    }
//    int res_w = block_write(handle, blocksize, add_document_block, buf_w);
//    free(buf_w);
//    if (res_w == -1) return DIR_ENTRY_ERROR;

//    return inode_id;
//}

// int del_document(char* name, vdisk_handle_t handle, uint32_t blocksize,
//                 uint16_t inode_id) {
//    /*输入inode号，从磁盘读出inode,从indoe的地址读出blcok，转成目录项数组，删除目录项，写回
//     */

//    uint32_t block_id = inode_id / (blocksize / 128);

//    void* buf = calloc(1, blocksize);
//    int res = block_read(handle, blocksize, block_id, buf);
//    if (res == -1) return DIR_ENTRY_ERROR;
//    /*取出一个blcok中的指定的inode */
//    inode_t* inode = NULL;
//    memcpy(inode, buf + (inode_id % (blocksize / 128) * 128),
//    sizeof(inode_t)); free(buf);

//    /*0号blaock和block的0号位置（记录当前目录）不会做给一般文件的目录项，这里
//     *这里将判重名文件之后添加记录项的blcok号，和block
//     *里面的第几个位置在判重过程中被改写
//     */
//    uint8_t site = 0;  // site这个地方就没用了，但是作为参数还得定义
//    int dir_num = blocksize / REC_LEN; /*一个块的最多目录项数目 */
//    for (uint32_t i = 0; i < inode->blocks;
//         i++) { /*遍历indoe说明的数据块个数 */

//        uint32_t block_id =
//            locate_block(handle, inode, i,
//                         blocksize);  //找到block_id,block_addr在inode.h文件

//        if (search_inode_addr(name, handle, block_id, blocksize, &site) !=
//            -1) {  //使用文件名找到了inode_id，要删除的文件名匹配到了
//            // printf("%s","文件已存在！");
//            dir_entry_t* dir = block_to_dir(
//                handle, block_id,
//                blocksize);  //将这个block 存的目录项表读出转成目录项表

//            for (int j = 0; j < dir_num; j++) {
//                if (strcmp(dir[j].name, name) == 0) {  //文件名相同
//                    // inode管理模块对该inode的释放

//                    dir[j].inode_addr =
//                        0;  //等于0表示该目录被删除 找空位也是这样判断
//                }
//            }
//        }
//    }
//    return inode_id;
//}

int create_dentry(vdisk_handle_t handle, uint16_t blocksize, uint8_t* bitmap,
                  block_stack_t* stack, dir_entry_t* parent, char* name,
                  uint8_t file_type) {
    /* 首先搜索其父目录下是否已有同名的项
     * 同时将找到的第一个空的dentry位置记下来
     */
    uint32_t free_dentry_block_at = 0;
    uint16_t free_dentry_offset = 0;

    if (parent != NULL) {
        inode_t* p_inode = load_inode(handle, blocksize, parent->inode);

        assert(blocksize % sizeof(dir_entry_t) == 0);
        dir_entry_t* dentries = malloc(blocksize);

        /* 每次读入一个block，遍历block中的dentries */
        for (uint32_t block = 0; block < p_inode->blocks; block++) {
            uint32_t block_addr =
                locate_block(handle, blocksize, p_inode, block);
            block_read(handle, blocksize, block_addr, dentries);
            for (uint16_t i = 0; i < blocksize / sizeof(dir_entry_t); i++) {
                if (strcmp(dentries[i].name, name) == 0) {
                    free(dentries);
                    return -1;
                } else if (dentries[i].file_type == FTYPE_UNUSED &&
                           free_dentry_block_at == 0) {
                    free_dentry_block_at = block_addr;
                    free_dentry_offset = i;
                }
            }
        }
        free(dentries);
    }

    /* 分配一个空闲盘块 */
    uint32_t block_no = data_block_alloc(handle, blocksize, stack);

    /* 信息填入inode */
    inode_t c_inode;
    c_inode.uid = 0;
    c_inode.mode = file_type;
    c_inode.size = blocksize;
    c_inode.atime = c_inode.ctime = c_inode.mtime = 0; /* TODO */
    c_inode.blocks = 1;
    c_inode.links_count = 1;
    c_inode.block = block_no;
    // inode_append_block(handle, blocksize, &c_inode); /* 把盘块添加给inode */

    uint16_t c_inode_no = inode_alloc(handle, blocksize, bitmap);
    dump_inode(handle, blocksize, c_inode_no, &c_inode); /* inode写入磁盘 */

    /* 信息填入目录项 */
    dir_entry_t c_dentry;
    strcpy(c_dentry.name, name);
    c_dentry.inode = c_inode_no;
    c_dentry.file_type = file_type;

    if (parent != NULL) {
        /* 把这个目录项加到父目录的block中，直接利用最开始找到的空闲dentry位置
         */
        dir_entry_t* dentries = (dir_entry_t*)malloc(blocksize);
        block_read(handle, blocksize, free_dentry_block_at, dentries);
        memcpy(&dentries[free_dentry_offset], &c_dentry, sizeof(dir_entry_t));
        free(dentries);
    }
}

// dir_entry_t* find_dentry(vdisk_handle_t handle, uint32_t blocksize,
//                         inode_t* inode, const char* filename) {
//    assert(blocksize % sizeof(dir_entry_t) == 0);
//    dir_entry_t* dentries = malloc(blocksize);

//    for (uint32_t block = 0; block < inode->blocks; block++) {
//        uint32_t block_addr = locate_block(handle, blocksize, inode, block);
//        block_read(handle, blocksize, block_addr, dentries);
//        for (uint16_t i = 0; i < blocksize / sizeof(dir_entry_t); i++) {
//            if (strcmp(dentries[i].name, filename) == 0) {
//                dir_entry_t* ret = malloc(sizeof(dir_entry_t));
//                memcpy(ret, &dentries[i], sizeof(dir_entry_t));
//                free(dentries);
//                return ret;
//            }
//        }
//    }
//    free(dentries);
//    return NULL;
//}
