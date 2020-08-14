#include <cstring>

#include "testtools.h"

extern "C" {
#include "virtual_disk.h"
}

TEST_CASE("VirtualDisk All", "[vdisk]") {
    /*
     * 测试Virtual Disk模块的全部功能
     */

    /* 创建两个文件用作虚拟磁盘 */
    const char vdisk1[] = "Testing/Temporary/vdisk1.fs";
    tt_create_diskfile(vdisk1, 32 * SECTOR_SIZE);
    const char vdisk2[] = "Testing/Temporary/vdisk2.fs";
    tt_create_diskfile(vdisk2, 2048 * SECTOR_SIZE);

    /* 将两个文件分别添加为vdisk1和vdisk2 */
    vdisk_handle_t handle1 = vdisk_add(vdisk1);
    vdisk_handle_t handle2 = vdisk_add(vdisk2);
    CAPTURE(handle1, handle2);

    /* 尝试重复添加vdisk2 */
    vdisk_handle_t handle2plus = vdisk_add(vdisk2);
    REQUIRE(handle2plus == -1);

    /* 测试获取vdisk设备大小 */
    REQUIRE(vdisk_get_size(handle1) == 32 * SECTOR_SIZE);
    REQUIRE(vdisk_get_size(handle2) == 2048 * SECTOR_SIZE);

    /* 生成32个扇区大小的随机内容 */
    char *buf_w = (char *)malloc(32 * SECTOR_SIZE * sizeof(char));
    srand((unsigned)time(NULL));
    for (int i = 0; i < 32 * SECTOR_SIZE; i++) {
        buf_w[i] = rand() % 256;
    }

    /* vdisk常规读写测试 */
    char *buf_r = (char *)malloc(16 * SECTOR_SIZE * sizeof(char));
    REQUIRE(vdisk_write(handle1, 0, 32, buf_w) == 32);
    /* 从6号扇区（第7个）开始读取两个扇区的内容 */
    REQUIRE(vdisk_read(handle1, 6, 2, buf_r) == 2);
    REQUIRE(strncmp(buf_r, buf_w + SECTOR_SIZE * 6, SECTOR_SIZE * 2) == 0);

    REQUIRE(vdisk_write(handle2, 1024, 32, buf_w) == 32);
    REQUIRE(vdisk_read(handle2, 1024, 8, buf_r) == 8);
    REQUIRE(strncmp(buf_r, buf_w, SECTOR_SIZE * 8) == 0);

    /* 边界条件读写测试 */
    REQUIRE(vdisk_write(handle1, 16, 32, buf_w) == 16);
    REQUIRE(vdisk_write(handle1, 31, 1, buf_w) == 1);
    REQUIRE(vdisk_write(handle1, 31, 2, buf_w) == 1);
    REQUIRE(vdisk_write(handle1, 32, 1, buf_w) == 0);
    REQUIRE(vdisk_write(handle1, 33, 1, buf_w) == 0);
    REQUIRE(vdisk_error(handle1) != 0);
    vdisk_clearerr(handle1);
    REQUIRE(vdisk_error(handle1) == 0);
    REQUIRE(vdisk_read(handle1, 16, 32, buf_w) == 16);
    REQUIRE(vdisk_read(handle1, 31, 1, buf_w) == 1);
    REQUIRE(vdisk_read(handle1, 31, 2, buf_w) == 1);
    REQUIRE(vdisk_read(handle1, 32, 1, buf_w) == 0);
    REQUIRE(vdisk_read(handle1, 33, 1, buf_w) == 0);
    REQUIRE(vdisk_error(handle1) != 0);
    vdisk_clearerr(handle1);
    REQUIRE(vdisk_error(handle1) == 0);

    /* 移除虚拟磁盘 */
    REQUIRE(vdisk_remove(handle1) == 0);
    REQUIRE(vdisk_remove(handle2) == 0);

    free(buf_w);
    free(buf_r);

    /* 删除临时文件 */
    remove(vdisk1);
    remove(vdisk2);
}
