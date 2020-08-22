#include "dir_entry.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"
#include "data_block.h"

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
                    //                    assert(0);
                    free_dentry_block_at = block_addr;
                    free_dentry_offset = i;
                }
            }
        }
        if (strcmp(name, "subdir") == 0) {
            assert(free_dentry_offset == 2);
        }
        assert(free_dentry_block_at != 0);
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

    //    assert(0);

    /* 为新建目录添加两个目录项：当前目录（.）和父目录（..） */
    if (file_type == FTYPE_DIR) {
        dir_entry_t* init_dentries = (dir_entry_t*)calloc(1, sb->block_size);
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
    //    assert(0);

    return 0;
}

int create_link(vdisk_handle_t handle, super_block_t* sb, dir_entry_t* parent,
                char* name, dir_entry_t* target) {
    uint32_t free_dentry_block_at = 0;
    uint16_t free_dentry_offset = 0;
    inode_t* p_inode = load_inode(handle, sb->block_size, parent->inode);

    dir_entry_t* dentries = malloc(sb->block_size);

    /* 每次读入一个block，遍历block中的dentries */
    for (uint32_t block = 0; block < p_inode->blocks; block++) {
        uint32_t block_addr =
            locate_block(handle, sb->block_size, p_inode, block);

        block_read(handle, sb->block_size, block_addr, dentries);
        for (uint16_t i = 0; i < sb->block_size / sizeof(dir_entry_t); i++) {
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

    dir_entry_t c_dentry;
    strcpy(c_dentry.name, name);
    c_dentry.inode = target->inode;
    c_dentry.file_type = FTYPE_LINK;

    block_read(handle, sb->block_size, free_dentry_block_at, dentries);
    memcpy(&dentries[free_dentry_offset], &c_dentry, sizeof(dir_entry_t));
    block_write(handle, sb->block_size, free_dentry_block_at, dentries);
    free(dentries);
    free(p_inode);

    inode_t* target_inode = load_inode(handle, sb->block_size, target->inode);
    target_inode->links_count++;
    dump_inode(handle, sb->block_size, target->inode, target_inode);

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

    //    assert(0);

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
    dump_inode(handle, sb->block_size, parent->inode,
               p_inode); /* inode写入磁盘 */

    free(dentries);
    return 0;
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
        //        free(inode_struct);
    } else if (dentry->file_type == FTYPE_FILE) {
        for (uint32_t i = 0; i < inode_struct->blocks; i++) {
            uint32_t block_addr =
                locate_block(handle, sb->block_size, inode_struct, i);
            data_block_free(handle, sb->block_size, block_addr,
                            &sb->group_stack);
            sb->free_blocks_count++;
        }
        inode_free(sb, bitmap, dentry->inode);
        sb->free_inodes_count++;
    } else if (dentry->file_type == FTYPE_DIR) {
        uint32_t blocks_count = inode_struct->blocks;

        dir_entry_t* dentries = malloc(sb->block_size);
        for (uint32_t i = 0; i < blocks_count; i++) {
            assert(blocks_count == 1);
            memset(dentries, 0, sb->block_size);
            uint32_t block =
                locate_block(handle, sb->block_size, inode_struct, i);
            block_read(handle, sb->block_size, block, dentries);
            for (uint16_t j = 0; j < sb->block_size / sizeof(dir_entry_t);
                 j++) {
                if (dentries[j].file_type != FTYPE_UNUSED &&
                    strcmp(dentries[j].name, ".") != 0 &&
                    strcmp(dentries[j].name, "..") != 0) {
                    // TODO: 这里要进行递归删除
                    //                    release_dentry(handle, sb, bitmap,
                    //                    &dentries[j]);
                }
            }
            data_block_free(handle, sb->block_size, block, &sb->group_stack);
            sb->free_blocks_count++;
        }
        free(dentries);
        inode_free(sb, bitmap, dentry->inode);
        sb->inodes_count++;
    }
    free(inode_struct);
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
