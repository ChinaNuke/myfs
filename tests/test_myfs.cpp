#include <cstring>

#include "testtools.h"

extern "C" {
#include "block.h"
#include "data_block.h"
#include "dir_entry.h"
#include "filesystem.h"
#include "super_block.h"
}

TEST_CASE("Block All", "[myfs][block]") {
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
    REQUIRE(block_write(handle, blocksize, 1, buf_w) == 0);
    REQUIRE(block_write(handle, blocksize, 2, buf_w + blocksize) == 0);
    char buf_r[blocksize];
    REQUIRE(block_read(handle, blocksize, 1, buf_r) == 0);
    REQUIRE(strncmp(buf_r, buf_w, blocksize) == 0);
    REQUIRE(block_read(handle, blocksize, 2, buf_r) == 0);
    REQUIRE(strncmp(buf_r, buf_w + blocksize, blocksize) == 0);

    /* 边界条件读写测试 */
    REQUIRE(block_write(handle, blocksize, 3, buf_w) == 0);
    REQUIRE(block_write(handle, blocksize, 4, buf_w) == BLOCK_ERROR);
    REQUIRE(block_write(handle, blocksize, 5, buf_w) == BLOCK_ERROR);
    REQUIRE(block_read(handle, blocksize, 3, buf_r) == 0);
    REQUIRE(block_read(handle, blocksize, 4, buf_r) == BLOCK_ERROR);
    REQUIRE(block_read(handle, blocksize, 5, buf_r) == BLOCK_ERROR);

    vdisk_remove(handle);
    remove(vdisk1);
}

TEST_CASE("Stack Dump and Load", "[myfs][group_stack]") {
    /*
     * 测试 dump_group() 和 load_group() 函数功能
     */

    const uint32_t blocksize = 8 * SECTOR_SIZE;
    const char vdisk1[] = "Testing/Temporary/vdisk_stack.fs";
    tt_create_diskfile(vdisk1, 4 * blocksize);
    vdisk_handle_t handle1 = vdisk_add(vdisk1);
    REQUIRE(handle1 != -1);

    /* 生成60个随机块地址 */
    uint32_t buf_w[60];
    srand((unsigned)time(NULL));
    for (int i = 0; i < 60; i++) {
        buf_w[i] = rand() % UINT32_MAX;
    }

    SECTION("Test stack dump and load") {
        block_stack_t stack1;
        for (uint8_t i = 0; i < 60; i++) {
            stack1.blocks[i] = buf_w[i];
        }
        stack1.top = 60 - 1;
        REQUIRE(stack1.blocks[20] == buf_w[20]);

        REQUIRE(dump_group(handle1, blocksize, 0, &stack1) == 0);

        block_stack_t stack2;
        REQUIRE(load_group(handle1, blocksize, 0, &stack2) == 0);

        REQUIRE(stack2.top == 60 - 1);
        for (uint8_t i = 0; i < 60; i++) {
            REQUIRE(stack2.blocks[i] == buf_w[i]);
        }
    }

    vdisk_remove(handle1);
    remove(vdisk1);
}

TEST_CASE("Data Block Initial", "[myfs][data_block]") {
    /*
     * 测试 data_blocks_init() 的功能
     */
    const uint32_t blocksize = 8 * SECTOR_SIZE;
    const char vdisk1[] = "Testing/Temporary/vdisk_data_block.fs";
    tt_create_diskfile(vdisk1, 500 * blocksize);
    vdisk_handle_t handle1 = vdisk_add(vdisk1);
    CAPTURE(handle1);

    SECTION("Initial 1 block") {
        uint32_t start = 0;
        uint32_t count = 1;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == count - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + count - 1);
    }
    SECTION("Initial another 1 block") {
        uint32_t start = 5;
        uint32_t count = 1;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == count - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + count - 1);
    }
    SECTION("Initial 2 blocks") {
        uint32_t start = 0;
        uint32_t count = 2;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == count - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + count - 1);
    }
    SECTION("Initial 99 blocks") {
        uint32_t start = 10;
        uint32_t count = 99;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == count - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + count - 1);
    }
    SECTION("Initial 100 blocks") {
        uint32_t start = 10;
        uint32_t count = 100;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == count - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + count - 1);
    }
    SECTION("Initial 101 blocks") {
        uint32_t start = 10;
        uint32_t count = 101;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == 100 - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + 100 - 1);
    }
    SECTION("Initial 200 blocks") {
        uint32_t start = 10;
        uint32_t count = 200;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == 100 - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + 100 - 1);
    }
    SECTION("Initial 250 blocks") {
        uint32_t start = 10;
        uint32_t count = 250;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == 100 - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + 100 - 1);
    }
    SECTION("Initial 300 blocks") {
        uint32_t start = 10;
        uint32_t count = 300;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == 100 - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + 100 - 1);
    }
    SECTION("Initial 500 blocks") {
        uint32_t start = 0;
        uint32_t count = 500;
        block_stack_t stack1;
        REQUIRE(data_blocks_init(handle1, blocksize, start, count, &stack1) ==
                0);
        REQUIRE(stack1.top == 100 - 1);
        REQUIRE(stack1.blocks[stack1.top] == start + 100 - 1);
    }

    vdisk_remove(handle1);
    remove(vdisk1);
}

TEST_CASE("Data Block Alloc and Free", "[myfs][data_block]") {
    /*
     * 测试 data_blocks_alloc() 和 data_blocks_free() 的功能
     */
    const uint32_t blocksize = 8 * SECTOR_SIZE;
    const char vdisk1[] = "Testing/Temporary/vdisk_data_block.fs";
    tt_create_diskfile(vdisk1, 500 * blocksize);
    vdisk_handle_t handle1 = vdisk_add(vdisk1);
    CAPTURE(handle1);

    /* 将后 300 个 blocks 初始化为 data blocks */
    block_stack_t stack1;
    REQUIRE(data_blocks_init(handle1, blocksize, 200, 300, &stack1) == 0);
    REQUIRE(stack1.top == 100 - 1);
    REQUIRE(stack1.blocks[stack1.top] == 200 + 99);

    SECTION("Simple Alloc and Free") {
        REQUIRE(data_block_alloc(handle1, blocksize, &stack1) == 200 + 99);
        REQUIRE(stack1.top == 98);
        REQUIRE(stack1.blocks[stack1.top] == 200 + 98);

        REQUIRE(data_block_alloc(handle1, blocksize, &stack1) == 200 + 98);
        REQUIRE(stack1.top == 97);
        REQUIRE(stack1.blocks[stack1.top] == 200 + 97);

        data_block_free(handle1, blocksize, 299, &stack1);
        REQUIRE(stack1.top == 98);
        REQUIRE(stack1.blocks[stack1.top] == 299);
    }

    SECTION("Empty and Full conditions") {
        for (int i = 0; i < 99; i++) {
            data_block_alloc(handle1, blocksize, &stack1);
        }
        REQUIRE(data_block_alloc(handle1, blocksize, &stack1) == 200);
        CAPTURE((int)(stack1.top));
        REQUIRE(stack1.top == 99);
        REQUIRE(stack1.blocks[stack1.top] == 399);

        data_block_free(handle1, blocksize, 250, &stack1);
        REQUIRE(stack1.top == 0);
        REQUIRE(stack1.blocks[stack1.top] == 250);
    }

    vdisk_remove(handle1);
    remove(vdisk1);
}

TEST_CASE("Filesystem Format and Create directory or file",
          "[myfs][filesystem]") {
    /*
     * 测试 myfs_format() 的功能
     */
    const uint16_t blocksize = BLK_SIZE_4K;
    const char vdisk1[] = "Testing/Temporary/vdisk_filesystem.fs";
    tt_create_diskfile(vdisk1, 2000 * blocksize); /* 8MB */
    vdisk_handle_t handle1 = vdisk_add(vdisk1);
    CAPTURE(handle1);

    REQUIRE(myfs_format(handle1, blocksize) == 0);

    vdisk_remove(handle1);
    handle1 = vdisk_add(vdisk1);

    /* 检查 superblock 区域 */
    super_block_t sb;
    load_super_block(handle1, &sb);
    REQUIRE(sb.block_size == blocksize);
    REQUIRE(sb.inode_size == INODE_SIZE);
    REQUIRE(sb.inodes_count == 8 * blocksize);
    REQUIRE(sb.free_inodes_count == 8 * blocksize - 1);
    REQUIRE(sb.total_size == 2000 * blocksize);

    /* 检查 bitmap 区域 */
    uint8_t *bitmap = (uint8_t *)malloc(blocksize);
    block_read(handle1, blocksize, 1, bitmap);
    REQUIRE(bitmap[0] == 0x01);
    for (uint16_t i = 1; i < blocksize / 8; i++) {
        REQUIRE(bitmap[i] == 0x00);
    }
    //    free(bitmap);

    /* 检查 inode table 区域 */
    char *buf = (char *)malloc(blocksize);
    block_read(handle1, blocksize, 2, buf);
    inode_t inode;
    memcpy(&inode, buf, sizeof(inode_t));
    REQUIRE(inode.uid == 0);
    REQUIRE(inode.mode == FTYPE_DIR);
    REQUIRE(inode.blocks == 1);
    REQUIRE(inode.atime == 0);
    REQUIRE(inode.ctime == 0);
    REQUIRE(inode.block == 1125);
    REQUIRE(inode.direct_blocks[0] == 1125);
    char *empty_block = (char *)calloc(1, blocksize);
    /* 检查剩下的1023个 inode 块 */
    for (uint16_t i = 3; i < 3 + 1023; i++) {
        block_read(handle1, blocksize, i, buf);
        REQUIRE(strncmp(buf, empty_block, blocksize) == 0);
    }
    free(buf);
    free(empty_block);

    /* 检查根目录的数据块内容 */
    dir_entry_t *dentries = (dir_entry_t *)malloc(blocksize);
    block_read(handle1, blocksize, inode.block, dentries);
    REQUIRE(strcmp(dentries[0].name, ".") == 0);
    REQUIRE(dentries[0].file_type == FTYPE_DIR);
    REQUIRE(dentries[0].inode == 0);
    for (uint16_t i = 1; i < blocksize / sizeof(dir_entry_t); i++) {
        REQUIRE(dentries[i].file_type == FTYPE_UNUSED);
    }
    //    free(dentries);

    REQUIRE(sizeof(dir_entry_t) == 32);

    /*
     * 测试在根目录创建目录/文件
     */

    REQUIRE(create_dentry(handle1, &sb, bitmap, &dentries[0],
                          (char *)"helloworld", FTYPE_DIR) == 0);
    REQUIRE(create_dentry(handle1, &sb, bitmap, &dentries[0],
                          (char *)"helloworld", FTYPE_DIR) == -1);
    REQUIRE(create_dentry(handle1, &sb, bitmap, &dentries[0], (char *)"myfs",
                          FTYPE_FILE) == 0);
    dump_super_block(handle1, &sb);

    dir_entry_t dir_root;
    memcpy(&dir_root, &dentries[0], sizeof(dir_entry_t));

    /* 再次检查 super block */
    REQUIRE(sb.free_inodes_count == 8 * blocksize - 3);

    block_read(handle1, blocksize, inode.block, dentries);
    REQUIRE(strcmp(dentries[0].name, ".") == 0);
    REQUIRE(dentries[0].file_type == FTYPE_DIR);
    REQUIRE(dentries[0].inode == 0);
    REQUIRE(strcmp(dentries[1].name, "helloworld") == 0);
    REQUIRE(dentries[1].file_type == FTYPE_DIR);
    REQUIRE(dentries[1].inode == 1);
    REQUIRE(strcmp(dentries[2].name, "myfs") == 0);
    REQUIRE(dentries[2].file_type == FTYPE_FILE);
    REQUIRE(dentries[2].inode == 2);
    for (uint16_t i = 3; i < blocksize / sizeof(dir_entry_t); i++) {
        REQUIRE(dentries[i].file_type == FTYPE_UNUSED);
    }

    dir_entry_t dir_helloworld;
    memcpy(&dir_helloworld, &dentries[1], sizeof(dir_entry_t));

    /* 子目录下创建目录/文件 */
    //    CAPTURE(dentries[1].name, dentries[1].file_type);
    REQUIRE(create_dentry(handle1, &sb, bitmap, &dir_helloworld,
                          (char *)"subdir", FTYPE_DIR) == 0);
    REQUIRE(create_dentry(handle1, &sb, bitmap, &dir_helloworld,
                          (char *)"subfile", FTYPE_FILE) == 0);
    REQUIRE(create_dentry(handle1, &sb, bitmap, &dir_helloworld,
                          (char *)"subdir", FTYPE_DIR) == -1);

    inode_t *helloworld = load_inode(handle1, blocksize, dir_helloworld.inode);
    REQUIRE(helloworld->blocks == 1);
    block_read(handle1, blocksize, helloworld->block, dentries);
    REQUIRE(strcmp(dentries[0].name, ".") == 0);
    REQUIRE(dentries[0].file_type == FTYPE_DIR);
    REQUIRE(dentries[1].file_type == FTYPE_DIR);
    REQUIRE(dentries[2].file_type == FTYPE_DIR);
    REQUIRE(dentries[3].file_type == FTYPE_FILE);
    REQUIRE(strcmp(dentries[1].name, "..") == 0);
    REQUIRE(strcmp(dentries[2].name, "subdir") == 0);
    REQUIRE(strcmp(dentries[3].name, "subfile") == 0);

    /* 测试删除文件/目录 */

    CAPTURE(dir_helloworld.name, dir_root.name);
    REQUIRE(remove_dentry(handle1, &sb, bitmap, &dir_helloworld, &dir_root) ==
            0);

    /* 再次检查 super block */
    //    REQUIRE(sb.free_inodes_count == 8 * blocksize - 1);

    block_read(handle1, blocksize, inode.block, dentries);
    REQUIRE(strcmp(dentries[0].name, ".") == 0);
    REQUIRE(dentries[0].file_type == FTYPE_DIR);
    REQUIRE(dentries[1].file_type == FTYPE_UNUSED);

    free(dentries);
    free(bitmap);
    vdisk_remove(handle1);
    remove(vdisk1);
}

TEST_CASE("Locate block for inode", "[myfs][inode]") {
    /*
     * 测试混合索引寻址
     */
    const uint16_t blocksize = BLK_SIZE_1K;
    const char vdisk1[] = "Testing/Temporary/vdisk_filesystem.fs";
    const uint32_t total_blocks = 2000;
    tt_create_diskfile(vdisk1, total_blocks * blocksize);
    vdisk_handle_t handle1 = vdisk_add(vdisk1);
    CAPTURE(handle1);
    REQUIRE(vdisk_error(handle1) == 0);

    const uint16_t addrs_per_block = blocksize / BLOCK_ADDR_LEN;

    inode_t inode1;
    srand((unsigned)time(NULL));

    SECTION("direct") {
        for (int i = 0; i < 12; i++) {
            inode1.direct_blocks[i] = rand() % total_blocks;
        }
        for (int i = 0; i < 12; i++) {
            REQUIRE(locate_block(handle1, blocksize, &inode1, i) ==
                    inode1.direct_blocks[i]);
        }
    }

    SECTION("single indirect") {
        uint32_t *buf = (uint32_t *)malloc(addrs_per_block * sizeof(uint32_t));
        for (uint16_t i = 0; i < addrs_per_block; i++) {
            buf[i] = rand() % total_blocks;
        }
        inode1.single_indirect = rand() % total_blocks;
        CAPTURE(inode1.single_indirect);
        REQUIRE(block_write(handle1, blocksize, inode1.single_indirect, buf) ==
                0);

        for (uint16_t i = 0; i < addrs_per_block; i++) {
            CAPTURE(i);
            REQUIRE(locate_block(handle1, blocksize, &inode1, 12 + i) ==
                    buf[i]);
        }
        free(buf);
    }

    SECTION("double indirect") {
        /* 一次间址块放在前半的blocks，以免被二次间址块覆盖 */
        inode1.double_indirect = rand() % 1000;
        CAPTURE(inode1.double_indirect);
        uint32_t *buf_1 =
            (uint32_t *)malloc(addrs_per_block * sizeof(uint32_t));
        for (uint16_t i = 0; i < addrs_per_block; i++) {
            /* 后半部分 */
            buf_1[i] = rand() % 1000 + 1000;
        }
        REQUIRE(block_write(handle1, blocksize, inode1.double_indirect,
                            buf_1) == 0);

        uint32_t *buf_2 =
            (uint32_t *)malloc(addrs_per_block * sizeof(uint32_t));
        for (uint16_t i = 0; i < addrs_per_block; i++) {
            buf_2[i] = rand() % 2000;
        }
        for (int i = 0; i < 50; i++) { /* 只对50个进行间址，减小测试量 */
            REQUIRE(block_write(handle1, blocksize, buf_1[i], buf_2) == 0);
        }

        for (uint32_t i = 0; i < 50 * addrs_per_block; i++) {
            CAPTURE(i);
            REQUIRE(locate_block(handle1, blocksize, &inode1,
                                 12 + addrs_per_block + i) ==
                    buf_2[i % addrs_per_block]);
        }

        free(buf_1);
        free(buf_2);
    }

    SECTION("triple indirect") {
        inode1.triple_indirect = rand() % 600;
        CAPTURE(inode1.triple_indirect);
        uint32_t *buf_1 =
            (uint32_t *)malloc(addrs_per_block * sizeof(uint32_t));
        for (uint16_t i = 0; i < addrs_per_block; i++) {
            buf_1[i] = rand() % 600 + 600;
        }
        REQUIRE(block_write(handle1, blocksize, inode1.triple_indirect,
                            buf_1) == 0);

        uint32_t *buf_2 =
            (uint32_t *)malloc(addrs_per_block * sizeof(uint32_t));
        for (uint16_t i = 0; i < addrs_per_block; i++) {
            buf_2[i] = rand() % 600 + 1200;
        }
        REQUIRE(block_write(handle1, blocksize, buf_1[0], buf_2) == 0);

        uint32_t *buf_3 =
            (uint32_t *)malloc(addrs_per_block * sizeof(uint32_t));
        for (uint16_t i = 0; i < addrs_per_block; i++) {
            buf_3[i] = rand() % 2000;
        }
        //        REQUIRE(block_write(handle1, blocksize, buf_2[0], buf_3) ==
        //        0);
        for (int i = 0; i < addrs_per_block; i++) {
            REQUIRE(block_write(handle1, blocksize, buf_2[i], buf_3) == 0);
        }

        for (uint32_t i = 0; i < addrs_per_block * addrs_per_block; i++) {
            CAPTURE(i);
            REQUIRE(locate_block(handle1, blocksize, &inode1,
                                 12 + addrs_per_block +
                                     addrs_per_block * addrs_per_block + i) ==
                    buf_3[i % addrs_per_block]);
        }

        free(buf_1);
        free(buf_2);
        free(buf_3);
    }

    vdisk_remove(handle1);
    remove(vdisk1);
}

TEST_CASE("Inode dump and load", "[myfs][inode]") {}
