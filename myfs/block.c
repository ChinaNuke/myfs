#include "block.h"

int block_read(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
               void *buf) {
    uint64_t sector_count = blocksize / SECTOR_SIZE;
    uint64_t sector_start = block * blocksize / SECTOR_SIZE;

    int64_t ret = vdisk_read(handle, sector_start, sector_count, buf);
    if (ret == 0) {
        /* 越界（VDISK_ERROR）或刚好在边界开始（0） */
        vdisk_clearerr(handle);
        return BLOCK_ERROR;
    } else {
        return 0;
    }
}

int block_write(vdisk_handle_t handle, uint32_t blocksize, uint32_t block,
                const void *buf) {
    uint64_t sector_count = blocksize / SECTOR_SIZE;
    uint64_t sector_start = block * blocksize / SECTOR_SIZE;

    int64_t ret = vdisk_write(handle, sector_start, sector_count, buf);
    if (ret == 0) {
        /* 越界（VDISK_ERROR）或刚好在边界开始（0） */
        vdisk_clearerr(handle);
        return BLOCK_ERROR;
    } else {
        return 0;
    }
}
