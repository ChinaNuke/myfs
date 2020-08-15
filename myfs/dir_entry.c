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

int create_dentry(vdisk_handle_t handle, super_block_t* sb, uint8_t* bitmap,
                  dir_entry_t* parent, char* name, uint8_t file_type) {
    /* 只可以在目录中创建目录项 */
    if (parent != NULL && parent->file_type != FTYPE_DIR) {
        //        assert(0);
        return -1;
    }

    /* 首先搜索其父目录下是否已有同名的项
     * 同时将找到的第一个空的dentry位置记下来
     */
    uint32_t free_dentry_block_at = 0;
    uint16_t free_dentry_offset = 0;

    if (parent != NULL) {
        inode_t* p_inode = load_inode(handle, sb->block_size, parent->inode);

        assert(sb->block_size % sizeof(dir_entry_t) == 0);
        dir_entry_t* dentries = malloc(sb->block_size);

        /* 每次读入一个block，遍历block中的dentries */
        for (uint32_t block = 0; block < p_inode->blocks; block++) {
            uint32_t block_addr =
                locate_block(handle, sb->block_size, p_inode, block);

            block_read(handle, sb->block_size, block_addr, dentries);
            for (uint16_t i = 0; i < sb->block_size / sizeof(dir_entry_t);
                 i++) {
                if (strcmp(dentries[i].name, name) == 0) {
                    //                    assert(0);
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

    /* 信息填入inode */
    inode_t c_inode;
    c_inode.uid = 0;
    c_inode.mode = file_type;
    c_inode.size = sb->block_size;
    c_inode.atime = c_inode.ctime = c_inode.mtime = 0; /* TODO */
    c_inode.blocks = 0;
    c_inode.links_count = 1;
    uint32_t block_no = inode_append_block(handle, sb, &c_inode);
    c_inode.block = block_no; /* 这个属性似乎没什么用 */

    int32_t c_inode_no = inode_alloc(sb, bitmap);
    assert(c_inode_no != INODE_ERROR);

    dump_inode(handle, sb->block_size, c_inode_no,
               &c_inode); /* inode写入磁盘 */

    /* 信息填入目录项 */
    dir_entry_t c_dentry;
    strcpy(c_dentry.name, name);
    c_dentry.inode = c_inode_no;
    c_dentry.file_type = file_type;

    if (parent != NULL) {
        /* 把这个目录项加到父目录的block中，直接利用最开始找到的空闲dentry位置
         */
        dir_entry_t* dentries = (dir_entry_t*)malloc(sb->block_size);
        block_read(handle, sb->block_size, free_dentry_block_at, dentries);
        memcpy(&dentries[free_dentry_offset], &c_dentry, sizeof(dir_entry_t));
        block_write(handle, sb->block_size, free_dentry_block_at, dentries);
        free(dentries);
    }

    /* 为新建目录添加两个目录项：当前目录（.）和父目录（..） */
    if (file_type == FTYPE_DIR) {
        dir_entry_t* init_dentries = (dir_entry_t*)malloc(sb->block_size);
        strcpy(init_dentries[0].name, ".");
        init_dentries[0].inode = c_inode_no;
        init_dentries[0].file_type = FTYPE_DIR;
        if (parent != NULL) {
            strcpy(init_dentries[1].name, "..");
            init_dentries[1].inode = parent->inode;
            init_dentries[1].file_type = FTYPE_DIR;
        }
        block_write(handle, sb->block_size, block_no, init_dentries);
        free(init_dentries);
    }

    return 0;
}
void release_dentry(vdisk_handle_t handle, super_block_t* sb, uint8_t* bitmap,
                    dir_entry_t* dentry);

int remove_dentry(vdisk_handle_t handle, super_block_t* sb, uint8_t* bitmap,
                  dir_entry_t* dentry, dir_entry_t* parent) {
    /* 不允许删除 ./.. 以及根目录 */
    if (strcmp(dentry->name, ".") == 0 || strcmp(dentry->name, "..") == 0 ||
        dentry->inode == 0) {
        return -1;
    }

    /* 对要删除的目录及其子目录执行递归释放 */
    release_dentry(handle, sb, bitmap, dentry);

    assert(0);

    /* 从其父目录中移除对应的目录项 */
    inode_t* p_inode = load_inode(handle, sb->block_size, parent->inode);
    dir_entry_t* dentries = malloc(sb->block_size);
    uint8_t found = 0;
    for (uint32_t block = 0; block < p_inode->blocks; block++) {
        memset(dentries, 0, sb->block_size);
        uint32_t block_addr =
            locate_block(handle, sb->block_size, p_inode, block);
        block_read(handle, sb->block_size, block_addr, dentries);
        for (uint16_t i = 0; i < sb->block_size / sizeof(dir_entry_t); i++) {
            if (dentries[i].file_type != FTYPE_UNUSED &&
                strcmp(dentries[i].name, dentry->name) == 0) {
                dentries[i].file_type = FTYPE_UNUSED;
                found = 1; /* 用于跳出外层循环 */
                break;
            }
        }
        if (found == 1) {
            block_write(handle, sb->block_size, block_addr, dentries);
            break;
        }
    }

    free(dentries);
    return 0;

    //    /* 每次读入一个block，遍历block中的dentries */
    //    for (uint32_t block = 0; block < p_inode->blocks; block++) {
    //        uint32_t block_addr =
    //            locate_block(handle, sb->block_size, p_inode, block);
    //        block_read(handle, sb->block_size, block_addr, dentries);
    //        for (uint16_t i = 0; i < sb->block_size / sizeof(dir_entry_t);
    //        i++) {
    //            if (strcmp(dentries[i].name, name) == 0) {
    //                if (file_type == FTYPE_DIR) { /*删除目录 */

    //                    release_dir(handle, sb, bitmap, dentries, i,
    //                    block_addr);
    //                } else if (file_type == FTYPE_FILE) { /*删除普通文件 */

    //                    release_file(handle, sb, bitmap, dentries, i,
    //                    block_addr);
    //                } else if (file_type == FTYPE_LINK) { /*删除链接文件 */
    //                    inode_t* free_inode =
    //                        load_inode(handle, sb->block_size,
    //                        dentries[i].inode);
    //                    if (free_inode->links_count > 1) {
    //                        /*正在被共享 */
    //                        free_inode->links_count--;
    //                    } else if (free_inode->links_count == 1) {
    //                        release_file(handle, sb, bitmap, dentries, i,
    //                                     block_addr);
    //                    } else {
    //                        free(dentries);
    //                        free(free_inode);
    //                        return -1;
    //                    }
    //                    /*删除成功 */
    //                    free(free_inode);
    //                    free(dentries);
    //                    // return ;
    //                }
    //                free(dentries);
    //            }
    //        }
    //    }
    //    free(dentries);

    //    return 0;
}
/* 删除目录(递归)
 */
void release_dentry(vdisk_handle_t handle, super_block_t* sb, uint8_t* bitmap,
                    dir_entry_t* dentry) {
    inode_t* inode_struct = load_inode(handle, sb->block_size, dentry->inode);

    /* 分为三种情况进行处理
     * 对于链接，只需要将其所指向的inode链接计数减1
     * 对于文件，需要将其所有盘块释放，并将其索引结点释放
     * 对于目录，要递归释放其子目录，最后释放其拥有的所有盘块，并释放其索引结点
     */

    if (dentry->file_type == FTYPE_LINK) {
        inode_struct->links_count--;
        /* TODO: 此处应考虑链接减小为0时删除文件/目录 */
        dump_inode(handle, sb->block_size, dentry->inode, inode_struct);
        free(inode_struct);
    } else if (dentry->file_type == FTYPE_FILE) {
        for (uint32_t i = 0; i < inode_struct->blocks; i++) {
            uint32_t block_addr =
                locate_block(handle, sb->block_size, inode_struct, i);
            data_block_free(handle, sb->block_size, block_addr,
                            &sb->group_stack);
        }
        inode_free(sb, bitmap, dentry->inode);
    } else if (dentry->file_type == FTYPE_DIR) {
        uint32_t blocks_count = inode_struct->blocks;

        dir_entry_t* dentries = malloc(sb->block_size);
        for (uint32_t i = 0; i < blocks_count; i++) {
            memset(dentries, 0, sb->block_size);
            uint32_t block =
                locate_block(handle, sb->block_size, inode_struct, i);
            block_read(handle, sb->block_size, block, dentries);
            for (uint16_t j = 0; j < sb->block_size / sizeof(dir_entry_t);
                 j++) {
                if (dentries[j].file_type != FTYPE_UNUSED &&
                    strcmp(dentries[j].name, ".") != 0 &&
                    strcmp(dentries[j].name, "..") != 0) {
                    release_dentry(handle, sb, bitmap, &dentries[j]);
                    assert(0);
                }
            }
            data_block_free(handle, sb->block_size, block, &sb->group_stack);
        }
        assert(0);
        free(dentries);
        inode_free(sb, bitmap, dentry->inode);
    }
    free(inode_struct);

    //    inode_t* free_inode =
    //        load_inode(handle, sb->block_size, dentries[offset].inode);
    //    if (free_inode->blocks == 0) { /*空目录or空文件(不占数据块) */
    //        /*释放inode */
    //        inode_free(sb, bitmap, dentries[offset].inode);
    //        /*清目录项记录 */
    //        dentries[offset].file_type = FTYPE_UNUSED;
    //        dentries[offset].inode = 0;
    //        //        dentries[offset].name=NULL;
    //        /*修改后写回 */
    //        block_write(handle, sb->block_size, block_addr, dentries);
    //        return;
    //    } else if (free_inode->blocks > 0) {
    //        if (dentries[offset].file_type == FTYPE_FILE) {
    //            release_file(handle, sb, bitmap, dentries, offset,
    //            block_addr); return;
    //        } else if (dentries[offset].file_type == FTYPE_LINK) {
    //            if (free_inode->links_count > 1) {
    //                /*正在被共享 */
    //                free_inode->links_count--;
    //                return;
    //            } else if (free_inode->links_count == 1) {
    //                release_file(handle, sb, bitmap, dentries, offset,
    //                block_addr); return;
    //            }
    //        } else {
    //            dir_entry_t* dentries = malloc(sb->block_size);
    //            for (uint32_t block = 0; block < free_inode->blocks; block++)
    //            {
    //                uint32_t block_id =
    //                    locate_block(handle, sb->block_size, free_inode,
    //                    block);
    //                block_read(handle, sb->block_size, block_id, dentries);
    //                release_dir(handle, sb, bitmap, dentries, block,
    //                block_id);
    //            }
    //        }
    //    }
}
/*remove_dentry的子函数
 *实现释放指定inode的数据块(存的是数据)
 */
// void release_file(vdisk_handle_t handle, super_block_t* sb, uint8_t* bitmap,
//                  dir_entry_t* dentries, uint16_t offset, uint32_t block_addr)
//                  {
//    inode_t* free_inode =
//        load_inode(handle, sb->block_size, dentries[offset].inode);
//    if (free_inode->blocks > 0) { /* 非空文件(占数据块)*/
//        for (uint32_t j = 0; j < free_inode->blocks; j++) {
//            uint32_t block_id =
//                locate_block(handle, sb->block_size, free_inode, j);
//            data_block_free(handle, sb->block_size, block_id,
//                            &(sb->group_stack));
//        }
//    }
//    /*释放inode */
//    inode_free(sb, bitmap, dentries[offset].inode);
//    /*清目录项记录 */
//    dentries[offset].file_type = FTYPE_UNUSED;
//    dentries[offset].inode = 0;
//    //    dentries[offset].name=NULL;
//    /*修改后写回 */
//    block_write(handle, sb->block_size, block_addr, dentries);
//}

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
