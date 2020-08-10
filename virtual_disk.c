#include "virtual_disk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/file_operation.h"

#define MAX_VDISK_COUNT 1024

typedef struct vdisk {
    FILE *fp;
    char *filepath;
    uint32_t sector_count;
} vdisk_t;

vdisk_t *vdisks[MAX_VDISK_COUNT] = {NULL};

vdisk_handle_t vdisk_add(const char *filepath) {
    /* 如果该文件已经被添加为vdisk，则不能重复添加 */
    for (vdisk_handle_t i = 0; i < MAX_VDISK_COUNT; i++) {
        if (vdisks[i] != NULL && strcmp(vdisks[i]->filepath, filepath) == 0) {
            return VDISK_ERROR;
        }
    }

    /* 从头开始寻找第一个空的位置 */
    for (vdisk_handle_t i = 0; i < MAX_VDISK_COUNT; i++) {
        if (vdisks[i] == NULL) {
            vdisks[i] = (vdisk_t *)malloc(sizeof(vdisk_t));
            vdisks[i]->filepath =
                (char *)malloc((strlen(filepath) + 1) * sizeof(char));
            strcpy(vdisks[i]->filepath, filepath);
            vdisks[i]->fp = fopen(filepath, "r+b");
            vdisks[i]->sector_count = u_filesize(vdisks[i]->fp) / SECTOR_SIZE;
            return i;
        }
    }

    return VDISK_ERROR;
}

int vdisk_remove(vdisk_handle_t handle) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
        return VDISK_ERROR;
    }
    fclose(vdisks[handle]->fp);
    free(vdisks[handle]->filepath);
    free(vdisks[handle]);
    vdisks[handle] = NULL;
    return 0;
}

int64_t vdisk_read(vdisk_handle_t handle, uint32_t sector, uint32_t count,
                   void *buf) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
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
        clearerr(vdisk->fp); /* 必须清除error，否则下一次必定还error */
        return VDISK_ERROR;
    } else {
        return count;
    }
}

int64_t vdisk_write(vdisk_handle_t handle, uint32_t sector, uint32_t count,
                    const void *buf) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
        return VDISK_ERROR;
    }

    /* 定位文件指针到要写入的起始扇区 */
    vdisk_t *vdisk = vdisks[handle];
    if (fseek(vdisk->fp, sector * SECTOR_SIZE, SEEK_SET) != 0) {
        return VDISK_ERROR;
    }

    if (sector + count > vdisk->sector_count) {
        count = vdisk->sector_count - sector;
    }
    fwrite(buf, SECTOR_SIZE, count, vdisk->fp);

    if (ferror(vdisk->fp)) {
        clearerr(vdisk->fp);
        return VDISK_ERROR;
    } else {
        return count;
    }
}
