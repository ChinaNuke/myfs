#include "virtual_disk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_VDISK_COUNT 1024

typedef struct vdisk {
    FILE *fp;
    char *filepath;
    uint64_t sector_count;
    int err_code; /* 0表示无错误，其他值表示有错误 */
} vdisk_t;

vdisk_t *vdisks[MAX_VDISK_COUNT] = {NULL};

vdisk_handle_t vdisk_add(const char *filepath) {
    /* 如果该文件已经被添加为vdisk，则不能重复添加 */
    for (vdisk_handle_t i = 0; i < MAX_VDISK_COUNT; i++) {
        if (vdisks[i] != NULL && strcmp(vdisks[i]->filepath, filepath) == 0) {
            return -1;
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
            vdisks[i]->sector_count = ut_filesize(vdisks[i]->fp) / SECTOR_SIZE;
            vdisks[i]->err_code = 0;
            return i;
        }
    }

    return -1;
}

int vdisk_remove(vdisk_handle_t handle) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
        return -1;
    }
    fclose(vdisks[handle]->fp);
    free(vdisks[handle]->filepath);
    free(vdisks[handle]);
    vdisks[handle] = NULL;
    return 0;
}

uint64_t vdisk_read(vdisk_handle_t handle, uint64_t sector, uint64_t count,
                    void *buf) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
        return 0;
    }
    vdisk_t *vdisk = vdisks[handle];

    //    vdisk_clearerr(handle);

    /* 起始扇区号越界检查 */
    if (sector >= vdisk->sector_count) {
        vdisk->err_code = 1;
        return 0;
    }

    /* 定位文件指针到要读取的起始扇区 */
    if (fseek(vdisk->fp, sector * SECTOR_SIZE, SEEK_SET) != 0) {
        vdisk->err_code = 1;
        return 0;
    }

    /* 可能会超过vdisk的总扇区数，这种情况下读取到结尾结束 */
    if (sector + count > vdisk->sector_count) {
        count = vdisk->sector_count - sector;
    }
    fread(buf, SECTOR_SIZE, count, vdisk->fp);

    if (ferror(vdisk->fp)) {
        clearerr(vdisk->fp); /* 必须清除error，否则下一次必定还error */
        vdisk->err_code = 1;
        return 0;
    } else {
        return count;
    }
}

uint64_t vdisk_write(vdisk_handle_t handle, uint64_t sector, uint64_t count,
                     const void *buf) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
        return 0;
    }
    vdisk_t *vdisk = vdisks[handle];

    //    vdisk_clearerr(handle);

    /* 起始扇区号越界检查 */
    if (sector >= vdisk->sector_count) {
        vdisk->err_code = 1;
        return 0;
    }

    /* 定位文件指针到要写入的起始扇区
     * 这里非常迷惑的一个事情，无论是否越界fseek都返回 0 !
     * 所以才有了上面那段越界检查 */
    if (fseek(vdisk->fp, sector * SECTOR_SIZE, SEEK_SET) != 0) {
        vdisk->err_code = 1;
        return 0;
    }

    if (sector + count > vdisk->sector_count) {
        count = vdisk->sector_count - sector;
    }
    fwrite(buf, SECTOR_SIZE, count, vdisk->fp);

    if (ferror(vdisk->fp)) {
        clearerr(vdisk->fp);
        vdisk->err_code = 1;
        return 0;
    } else {
        return count;
    }
}

uint64_t vdisk_get_size(vdisk_handle_t handle) {
    if (vdisks[handle] != NULL) {
        return vdisks[handle]->sector_count * SECTOR_SIZE;
    } else {
        return 0;
    }
}

int vdisk_error(vdisk_handle_t handle) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
        return 1;
    }
    return vdisks[handle]->err_code;
}

void vdisk_clearerr(vdisk_handle_t handle) {
    if (handle < 0 || handle >= MAX_VDISK_COUNT || vdisks[handle] == NULL) {
        return;
    }
    vdisks[handle]->err_code = 0;
}
