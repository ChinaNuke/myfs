#include <cstring>

#include "testtools.h"

extern "C" {
#include "myfs/block.h"
}

TEST_CASE("Block All", "[block]") {
    /*
     * 测试 Block 的所有内容
     * 假定一个 block 有 8 个扇区
     */

    const uint32_t blocksize = 8 * SECTOR_SIZE;

    const char vdisk1[] = "Testing/Temporary/vdisk_block.fs";
    tt_create_diskfile(vdisk1, 4 * blocksize);
    vdisk_handle_t handle = vdisk_add(vdisk1);
    CAPTURE(handle);

    /* 生成2个块大小的随机内容 */
    char buf_w[2 * blocksize];
    srand((unsigned)time(NULL));
    for (uint32_t i = 0; i < 2 * blocksize; i++) {
        buf_w[i] = rand() % 256;
    }

    /* 常规读写测试 */
    REQUIRE(block_write(handle, 1, blocksize, buf_w) == 0);
    REQUIRE(block_write(handle, 2, blocksize, buf_w + blocksize) == 0);
    char buf_r[blocksize];
    REQUIRE(block_read(handle, 1, blocksize, buf_r) == 0);
    REQUIRE(strncmp(buf_r, buf_w, blocksize) == 0);
    REQUIRE(block_read(handle, 2, blocksize, buf_r) == 0);
    REQUIRE(strncmp(buf_r, buf_w + blocksize, blocksize) == 0);

    /* 边界条件读写测试 */
    REQUIRE(block_write(handle, 3, blocksize, buf_w) == 0);
    REQUIRE(block_write(handle, 4, blocksize, buf_w) == BLOCK_ERROR);
    REQUIRE(block_write(handle, 5, blocksize, buf_w) == BLOCK_ERROR);
    REQUIRE(block_read(handle, 3, blocksize, buf_r) == 0);
    REQUIRE(block_read(handle, 4, blocksize, buf_r) == BLOCK_ERROR);
    REQUIRE(block_read(handle, 5, blocksize, buf_r) == BLOCK_ERROR);

    vdisk_remove(handle);
    remove(vdisk1);
}
