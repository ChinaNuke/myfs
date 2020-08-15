#include "super_block.h"

#include <stdlib.h>
#include <string.h>

#include "block.h"

int load_super_block(vdisk_handle_t handle, super_block_t *sb) {
    void *buf = malloc(BLK_SIZE_1K);
    if (block_read(handle, BLK_SIZE_1K, 0, buf) != 0) {
        return -1;
    }
    memcpy(sb, buf, sizeof(super_block_t));
    free(buf);
    return 0;
}

int dump_super_block(vdisk_handle_t handle, super_block_t *sb) {
    void *buf = malloc(BLK_SIZE_1K);
    memcpy(buf, sb, sizeof(super_block_t));
    if (block_write(handle, BLK_SIZE_1K, 0, buf) != 0) {
        return -1;
    }
    free(buf);
    return 0;
}
