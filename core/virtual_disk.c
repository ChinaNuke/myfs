#include "virtual_disk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/file_operation.h"

#define MAX_DISK_COUNT 1024

typedef struct vdisk {
    FILE *fp;
    char *filepath;
    uint64_t sector_count;
} vdisk_t;

vdisk_t *vdisk_handles[MAX_DISK_COUNT] = {NULL};
vdisk_handle_t h_top = 0; /* 指向最后一个vdisk之后的空位置 */

vdisk_handle_t vdisk_add(const char *filepath) {
    /* 如果当前所管理的最大vdisk数达到了上限，则不能再添加新的vdisk */
    if (h_top >= MAX_DISK_COUNT) {
        return DISK_ERROR;
    }
    /* 如果该文件已经被添加为vdisk，则不能重复添加 */
    for (vdisk_handle_t i = 0; i < h_top; i++) {
        if (vdisk_handles[i]->filepath == filepath) {
            return DISK_ERROR;
        }
    }

    vdisk_handles[h_top] = (vdisk_t *)malloc(sizeof(vdisk_t));

    vdisk_handles[h_top]->filepath =
        (char *)malloc((strlen(filepath) + 1) * sizeof(char));
    strcpy(vdisk_handles[h_top]->filepath, filepath);

    vdisk_handles[h_top]->fp = fopen(filepath, "r+b");

    vdisk_handles[h_top]->sector_count =
        u_filesize(vdisk_handles[h_top]->fp) / SECTOR_SIZE;

    return h_top++;
}

int vdisk_remove(vdisk_handle_t handle) {
    if (handle < 0 || handle >= h_top) {
        return DISK_ERROR;
    }
    fclose(vdisk_handles[handle]->fp);
    free(vdisk_handles[handle]->filepath);
    free(vdisk_handles[handle]);
    vdisk_handles[handle] = NULL;
    return 0;
}
