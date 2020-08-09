#include <cstring>

#include "catch.hpp"

extern "C" {
#include "virtual_disk.h"
}

TEST_CASE("VirtualDisk All", "[vdisk]") {
    /*
     * 测试Virtual Disk模块的全部功能
     */

    /* 创建两个文件用作虚拟磁盘 */
    const char vdisk1[] = "Testing/Temporary/vdisk1.fs";
    char buf1[32 * SECTOR_SIZE] = {0}; /* 32个扇区，即16KB的vdisk文件 */
    FILE* fp1 =
        fopen(vdisk1, "w+b"); /* 这里必须用w+而不能用r+，因为文件事先不存在 */
    fwrite(buf1, SECTOR_SIZE, 32, fp1);
    fclose(fp1);

    const char vdisk2[] = "Testing/Temporary/vdisk2.fs";
    char buf2[2048 * SECTOR_SIZE] = {0}; /* 2048个扇区，即1MB的vdisk文件 */
    FILE* fp2 = fopen(vdisk2, "w+b");
    fwrite(buf2, SECTOR_SIZE, 2048, fp2);
    fclose(fp2);

    /* 将两个文件分别添加为vdisk1和vdisk2 */
    vdisk_handle_t handle1 = vdisk_add(vdisk1);
    REQUIRE(handle1 == 0);
    vdisk_handle_t handle2 = vdisk_add(vdisk2);
    REQUIRE(handle2 == 1);

    /* 尝试重复添加vdisk2 */
    vdisk_handle_t handle2plus = vdisk_add(vdisk2);
    REQUIRE(handle2plus == VDISK_ERROR);

    /* 生成32个扇区大小的随机内容 */
    char buf_w[32 * SECTOR_SIZE];
    srand((unsigned)time(NULL));
    for (int i = 0; i < 32 * SECTOR_SIZE; i++) {
        buf_w[i] = rand() % 256;
    }

    /* vdisk读写测试 */
    vdisk_write(handle1, 0, 32, buf_w);
    vdisk_read(handle1, 6, 2,
               buf1); /* 从6号扇区（第7个）开始读取两个扇区的内容 */
    REQUIRE(strncmp(buf1, buf_w + SECTOR_SIZE * 6, SECTOR_SIZE * 2) == 0);

    vdisk_write(handle2, 1024, 32, buf_w);
    vdisk_read(handle2, 1024, 8, buf2);
    REQUIRE(strncmp(buf2, buf_w, SECTOR_SIZE * 8) == 0);

    /* 移除虚拟磁盘 */
    REQUIRE(vdisk_remove(handle1) == 0);
    REQUIRE(vdisk_remove(handle2) == 0);

    /* 删除临时文件 */
    remove(vdisk1);
    remove(vdisk2);
}
