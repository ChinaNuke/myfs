#include "fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "virtual_disk.h"

int fs_format(char *filename, uint16_t blocksize) {
    vdisk_handle_t handle = vdisk_add(filename);
    if (handle < 0) {
        return -1;
    }
    myfs_format(handle, blocksize);
    vdisk_remove(handle);
    return 0;
}

char fs_mount(char *filename) {
    vdisk_handle_t handle = vdisk_add(filename);
    if (handle < 0) {
        return -1;
    }
    for (uint8_t i = 0; i < MAX_FS_COUNT; i++) {
        if (filesystems[i] == NULL) {
            filesystems[i] = malloc(sizeof(myfs_t));
            if (myfs_mount(handle, &filesystems[i]) == 0) {
                strcpy(filenames[i], filename);
                if (strcmp(cur_dir, "") == 0) {
                    cur_dir[0] = 'A' + i;
                    strcpy(cur_dir + 1, ":");
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
    };
    return -1;
}

char *fs_pwd() { return cur_dir; }

void *fs_disks() {
    for (uint8_t i = 0; i < MAX_FS_COUNT; i++) {
        if (filesystems[i] != NULL) {
            printf("%c\t%s\n", 'A' + i, filenames[i]);
        }
    }
}

void *fs_ls() {}
