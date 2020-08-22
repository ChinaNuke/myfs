#include "filesystem.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dir_entry.h"

/* 除法向上取整 */
#define CEIL_DIVIDE(A, B) (((A) + (B)-1) / (B))

int myfs_format(vdisk_handle_t handle, uint16_t blocksize) {
    /*
     * blocksize 一般为 1KB/2KB/4KB
     *
     * 0 号块：super block
     * 1 号块：inode bitmap
     */

    uint32_t device_size = vdisk_get_size(handle); /* 设备大小（字节） */
    uint32_t total_blocks = device_size / blocksize; /* 多出来的不使用 */

    /* 初始化 inode table 区
     * 默认 inode bitmap 固定占一个 block，即紧随 superblock 之后的 1 号块
     * 在 blocksize 为 4096 的情况下，最多支持 32768 个文件
     */

    void *empty_block = calloc(1, blocksize);

    uint16_t inodes_count = 1 * blocksize * 8;
    uint32_t inodes_size = inodes_count * INODE_SIZE;
    uint32_t inode_blocks = CEIL_DIVIDE(inodes_size, blocksize);

    //    printf("inode_blocks:%d, blocksize: %d, handle: %d\n", inode_blocks,
    //           blocksize, handle);

    //    assert(0);

    /* 写入 inode table */
    for (uint32_t i = 0; i < inode_blocks; i++) {
        if (block_write(handle, blocksize, 2 + i, empty_block) == BLOCK_ERROR) {
            return MYFS_ERROR;
        }
    }
    free(empty_block);

    /* 初始化 super block */
    super_block_t sb; /* 写在前面因为要用它的 first_group_stack */
    sb.block_size = blocksize;
    sb.inode_size = INODE_SIZE;
    sb.inodes_count = sb.free_inodes_count = inodes_count;
    sb.total_size = total_blocks * blocksize;

    /* 初始化 data block 区 */
    uint32_t data_blocks_start = 2 + inode_blocks;
    uint32_t data_blocks_count = total_blocks - data_blocks_start;
    if (data_blocks_init(handle, blocksize, data_blocks_start,
                         data_blocks_count, &(sb.group_stack)) == BLOCK_ERROR) {
        return MYFS_ERROR;
    }

    assert(sb.free_inodes_count == 8 * blocksize);

    /* 建立根目录 */
    uint8_t *bitmap = calloc(1, blocksize);
    create_dentry(handle, &sb, bitmap, NULL, "", FTYPE_DIR);

    block_write(handle, blocksize, 1, bitmap); /* 写入 bitmap */
    free(bitmap);

    sb.blocks_count = sb.free_blocks_count = data_blocks_count;

    // sb.first_data_block = 1;

    void *buf = calloc(1, blocksize);
    memcpy(buf, &sb, sizeof(super_block_t));
    if (block_write(handle, blocksize, 0, buf) == BLOCK_ERROR) {
        return MYFS_ERROR;
    }
    free(buf);

    return 0;
}

int myfs_mount(vdisk_handle_t handle, myfs_t *fs) {
    super_block_t *sblock = (super_block_t *)malloc(sizeof(super_block_t));

    /* 默认 super block 大小不会超过1K，所以在不知道 blocksize 的情况下按 1K
     * 读出 super block */
    load_super_block(handle, sblock);
    assert(sblock->block_size > 0);
    assert(sblock->blocks_count > 0);
    void *bitmap = calloc(1, sblock->block_size);
    block_read(handle, sblock->block_size, 1, bitmap);

    fs->sb = sblock;
    fs->disk_handle = handle;
    fs->bitmap = bitmap;
    return 0;
}

int myfs_unmount(myfs_t *fs) {
    free(fs->bitmap);
    free(fs->sb);
    //    free(fs);
    return 0;
}

dir_entry_t *myfs_parse_path(myfs_t *fs, dir_entry_t *cur_dir,
                             const char *path);

int myfs_mkdir(myfs_t *fs, dir_entry_t *cur_dir, char *path) {
    /* 这里暂且假设path只是一个目录名，不含路径 */
    //    printf("%s", path);
    int ret = create_dentry(fs->disk_handle, fs->sb, fs->bitmap, cur_dir, path,
                            FTYPE_DIR);
    return ret;
}

int myfs_touch(myfs_t *fs, dir_entry_t *cur_dir, char *path) {
    int ret = create_dentry(fs->disk_handle, fs->sb, fs->bitmap, cur_dir, path,
                            FTYPE_FILE);
    return ret;
}

int myfs_ls(myfs_t *fs, dir_entry_t *cur_dir) {
    uint16_t blocksize = fs->sb->block_size;
    inode_t *inode = load_inode(fs->disk_handle, blocksize, cur_dir->inode);
    dir_entry_t *dentries = malloc(blocksize);
    for (uint32_t i = 0; i < inode->blocks; i++) {
        uint32_t block_addr =
            locate_block(fs->disk_handle, blocksize, inode, i);
        block_read(fs->disk_handle, blocksize, block_addr, dentries);
        for (uint16_t j = 0; j < blocksize / sizeof(dir_entry_t); j++) {
            if (dentries[j].file_type == FTYPE_DIR) {
                printf("\033[33m%s\033[0m\t", dentries[j].name);
            } else if (dentries[j].file_type == FTYPE_FILE) {
                printf("%s\t", dentries[j].name);
            } else if (dentries[j].file_type == FTYPE_LINK) {
                printf("\033[36m%s\033[0m\t", dentries[j].name);
            }
        }
    }
    printf("\n");
    free(dentries);
    return 0;
}

int myfs_rm(myfs_t *fs, dir_entry_t *cur_dir, char *path) {
    dir_entry_t *to_remove = myfs_search_dentry(fs, cur_dir, path);
    if (to_remove == NULL) {
        return -1;
    }
    remove_dentry(fs->disk_handle, fs->sb, fs->bitmap, to_remove, cur_dir);
    return 0;
}

int myfs_stat(myfs_t *fs, dir_entry_t *cur_dir, char *path) {
    dir_entry_t *target = myfs_search_dentry(fs, cur_dir, path);
    if (target == NULL) {
        assert(0);
        return -1;
    }
    inode_t *inode =
        load_inode(fs->disk_handle, fs->sb->block_size, target->inode);
    printf("\033[33m%s:\033[0m\n", path);
    printf("拥有者：%d\n", inode->uid);
    if (inode->mode == FTYPE_DIR) {
        printf("类型：目录\n");
    } else if (inode->mode == FTYPE_FILE) {
        printf("类型：文件\n");
    } else if (inode->mode == FTYPE_LINK) {
        printf("类型：链接\n");
    }
    printf("大小：%d字节\n", inode->size);
    printf("创建时间：%d\n", inode->ctime);
    printf("修改时间：%d\n", inode->mtime);
    printf("访问时间：%d\n", inode->atime);
    printf("链接数：%d\n", inode->links_count);
    printf("占用数据块数：%d\n", inode->blocks);

    free(inode);

    return 0;
}

int myfs_link(myfs_t *fs, char *link, dir_entry_t *cur_dir, char *targetname) {
    dir_entry_t *target = myfs_search_dentry(fs, cur_dir, targetname);
    if (target == NULL) {
        assert(0);
        return -1;
    }
    create_link(fs->disk_handle, fs->sb, cur_dir, link, target);
    return 0;
}

/* PRIVATE */

dir_entry_t *myfs_search_dentry(myfs_t *fs, dir_entry_t *parent,
                                const char *dentryname) {
    inode_t *inode =
        load_inode(fs->disk_handle, fs->sb->block_size, parent->inode);
    dir_entry_t *buf = malloc(fs->sb->block_size);
    assert(inode->blocks == 1);
    for (uint32_t i = 0; i < inode->blocks; i++) {
        uint32_t block_addr =
            locate_block(fs->disk_handle, fs->sb->block_size, inode, i);
        block_read(fs->disk_handle, fs->sb->block_size, block_addr, buf);
        for (uint16_t j = 0; j < (fs->sb->block_size) / sizeof(dir_entry_t);
             j++) {
            assert(j < 128);
            if (buf[j].file_type != FTYPE_UNUSED &&
                strcmp(buf[j].name, dentryname) == 0) {
                dir_entry_t *ret = malloc(sizeof(dir_entry_t));
                memcpy(ret, &buf[j], sizeof(dir_entry_t));
                free(buf);
                return ret;
            }
        }
    }
    return NULL;
}

dir_entry_t *myfs_parse_path(myfs_t *fs, dir_entry_t *cur_dir,
                             const char *path) {
    dir_entry_t *current = malloc(sizeof(dir_entry_t));
    const char *relative_path;
    if (strlen(path) > 0 && path[0] != '/') {
        memcpy(current, cur_dir, sizeof(dir_entry_t));
        relative_path = path + 1;
    } else {
        //        current = ;  // 根目录
        assert(0);
        relative_path = path;
    }

    char *first_slash_pos;
    char *subpath = (char *)relative_path;
    char subdir[1024];
    int endflag = 0;

    while (endflag == 0) {
        first_slash_pos = strchr(subpath, '/');
        if (first_slash_pos == NULL) {
            strcpy(subdir, subpath);
            endflag = 1;
        } else {
            int length = first_slash_pos - subpath + 1;
            strncpy(subdir, subpath, length);
            subdir[length] = '\0';
        }
        current = myfs_search_dentry(fs, current, subdir);
    }

    return current;
}
