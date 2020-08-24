#include "fs.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "virtual_disk.h"

int fs_format(char *filename, uint16_t blocksize) {
    vdisk_handle_t handle = vdisk_add(filename);
    if (handle < 0) {
        return -1;
    }
    int ret = myfs_format(handle, blocksize);
    vdisk_remove(handle);
    return ret;
}

char fs_mount(char *filename) {
    vdisk_handle_t handle = vdisk_add(filename);
    if (handle < 0) {
        return -1;
    }
    for (uint8_t i = 0; i < MAX_FS_COUNT; i++) {
        if (filesystems[i] == NULL) {
            filesystems[i] = malloc(sizeof(myfs_t));
            if (myfs_mount(handle, filesystems[i]) == 0) {
                strcpy(filenames[i], filename);
                if (strcmp(cur_dir, "") == 0) {
                    cur_dir[0] = 'A' + i;
                    strcpy(cur_dir + 1, ":/");
                    dir_entry_t *root = malloc(sizeof(dir_entry_t));
                    strcpy(root->name, "");
                    root->inode = 0;
                    root->file_type = FTYPE_DIR;
                    cur_dentry = root;
                }
                return 'A' + i;
            }
        }
    }
    return -1;
}

int fs_unmount(char c) {
    if (c < 'A' || c > 'Z') {
        return -1;
    }
    int i = c - 'A';
    if (filesystems[i] != NULL && myfs_unmount(filesystems[i]) == 0) {
        vdisk_remove(filesystems[i]->disk_handle);
        filesystems[i] = NULL;
        free(filesystems[i]);
        if (cur_dir[0] == c) {
            strcpy(cur_dir, "");
        }
        return 0;
    }
    return -1;
}

char *fs_pwd() { return cur_dir; }

void fs_df() {
    printf("盘符\t磁盘文件名\t设备号\t块大小\t盘块数\t\t索引结点数\n");
    for (uint8_t i = 0; i < MAX_FS_COUNT; i++) {
        if (filesystems[i] != NULL) {
            printf("%c\t%-10s\t%d\t%d\t%d/%d\t%d/%d\t\n", 'A' + i, filenames[i],
                   filesystems[i]->disk_handle, filesystems[i]->sb->block_size,
                   filesystems[i]->sb->free_blocks_count,
                   filesystems[i]->sb->blocks_count,
                   filesystems[i]->sb->free_inodes_count,
                   filesystems[i]->sb->inodes_count);
        }
    }
}

int fs_chdisk(char c) {
    if (c < 'A' || c > 'Z') {
        return -1;
    }
    int i = c - 'A';
    if (filesystems[i] != NULL) {
        cur_fs = i;
        cur_dir[0] = 'A' + i;
        strcpy(cur_dir + 1, ":");
        return 0;
    }
    return -1;
}

int fs_mkdir(char *path) {
    //    assert(0);
    return myfs_mkdir(filesystems[cur_fs], cur_dentry, path);
}

int fs_touch(char *path) {
    return myfs_touch(filesystems[cur_fs], cur_dentry, path);
}

int fs_ls() {
    int ret = myfs_ls(filesystems[cur_fs], cur_dentry);
    return ret;
}

int fs_rm(char *path) { return myfs_rm(filesystems[cur_fs], cur_dentry, path); }

int fs_cd(char *path) {
    dir_entry_t *target =
        myfs_search_dentry(filesystems[cur_fs], cur_dentry, path);
    if (target != NULL && target->file_type == FTYPE_DIR) {
        free(cur_dentry);
        cur_dentry = target;
        if (strcmp(path, "..") == 0) {
            char *last_slash = strrchr(cur_dir, '/');
            assert(last_slash != NULL);
            *last_slash = '\0';
            char *last_last_slash = strrchr(cur_dir, '/');
            *(last_last_slash + 1) = '\0';
        } else if (strcmp(path, ".") != 0) {
            strcat(cur_dir, path);
            strcat(cur_dir, "/");
        }
        return 0;
    }
    return -1;
}

int fs_stat(char *path) {
    myfs_stat(filesystems[cur_fs], cur_dentry, path);
    return 0;
}

int fs_link(char *link, char *target) {
    return myfs_link(filesystems[cur_fs], link, cur_dentry, target);
}

int fs_chmod(char *path, uint16_t perm) {
    return myfs_chmod(filesystems[cur_fs], cur_dentry, path, perm);
}

int fs_mv(char *from, char *to) {
    if (fs_link(to, from) == 0 && fs_rm(from) == 0) {
        return 0;
    }
    return -1;
}
