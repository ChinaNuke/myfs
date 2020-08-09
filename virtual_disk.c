#include "virtual_disk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/file_operation.h"

#define MAX_VDISK_COUNT 1024

typedef struct vdisk {
    FILE *fp;
    char *filepath;
    uint64_t sector_count;
} vdisk_t;

vdisk_t *vdisks[MAX_VDISK_COUNT] = {NULL};
vdisk_handle_t h_top = 0; /* 指向最后一个vdisk之后的空位置 */

vdisk_handle_t vdisk_add(const char *filepath) {
    /* 如果当前所管理的最大vdisk数达到了上限，则不能再添加新的vdisk */
    if (h_top >= MAX_VDISK_COUNT) {
        return VDISK_ERROR;
    }
    /* 如果该文件已经被添加为vdisk，则不能重复添加 */
    for (vdisk_handle_t i = 0; i < h_top; i++) {
        if (strcmp(vdisks[i]->filepath, filepath) == 0) {
            return VDISK_ERROR;
        }
    }

    vdisks[h_top] = (vdisk_t *)malloc(sizeof(vdisk_t));

    vdisks[h_top]->filepath =
        (char *)malloc((strlen(filepath) + 1) * sizeof(char));
    strcpy(vdisks[h_top]->filepath, filepath);

    vdisks[h_top]->fp = fopen(filepath, "r+b");

    vdisks[h_top]->sector_count = u_filesize(vdisks[h_top]->fp) / SECTOR_SIZE;

    return h_top++;
}

int vdisk_remove(vdisk_handle_t handle) {
    if (handle < 0 || handle >= h_top) {
        return VDISK_ERROR;
    }
    fclose(vdisks[handle]->fp);
    free(vdisks[handle]->filepath);
    free(vdisks[handle]);
    vdisks[handle] = NULL;
    return 0;
}

uint64_t vdisk_read(vdisk_handle_t handle, uint64_t sector, uint64_t count,
                    char *buf) {
    if (handle < 0 || handle >= h_top) {
        return VDISK_ERROR;
    }

    /* 定位文件指针到要读取的起始扇区 */
    vdisk_t *vdisk = vdisks[handle];
    if (fseek(vdisk->fp, sector * SECTOR_SIZE, SEEK_SET) != 0) {
        return VDISK_ERROR;
    }

    /* 可能会超过vdisk的总扇区数，这种情况下读取到结尾结束 */
    if (sector + count > vdisk->sector_count) {
        count = vdisk->sector_count - sector;
    }
    fread(buf, SECTOR_SIZE, count, vdisk->fp);

    if (ferror(vdisk->fp)) {
        return VDISK_ERROR;
    } else {
        return count;
    }
}

uint64_t vdisk_write(vdisk_handle_t handle, uint64_t sector, uint64_t count,
                     const char *buf) {
    if (handle < 0 || handle >= h_top) {
        return VDISK_ERROR;
    }

    /* 定位文件指针到要读取的起始扇区 */
    vdisk_t *vdisk = vdisks[handle];
    if (fseek(vdisk->fp, sector * SECTOR_SIZE, SEEK_SET) != 0) {
        return VDISK_ERROR;
    }

    if (sector + count > vdisk->sector_count) {
        count = vdisk->sector_count - sector;
    }
    fwrite(buf, SECTOR_SIZE, count, vdisk->fp);

    if (ferror(vdisk->fp)) {
        return VDISK_ERROR;
    } else {
        return count;
    }
}
