#ifndef __VIRTUAL_DISK__H__
#define __VIRTUAL_DISK__H__

#include <stdint.h>

/*
 * Virtual Disk 模块（vdisk）
 * 对虚拟的磁盘文件进行读写等操作
 * 以扇区为操作单位，每个扇区512个字节
 *
 * 扇区号32位，最大支持2TB的硬盘
 *
 */

#define SECTOR_SIZE 512
#define VDISK_ERROR -1

typedef int16_t vdisk_handle_t; /* 虚拟磁盘句柄 */

/* 将一个文件添加为虚拟磁盘 */
vdisk_handle_t vdisk_add(const char* filepath);

/* 移除一个虚拟磁盘（不删除文件） */
int vdisk_remove(vdisk_handle_t handle);

/* 从虚拟磁盘读入数据（以扇区为单位） */
int64_t vdisk_read(vdisk_handle_t handle, uint32_t sector, uint32_t count,
                   void* buf);

/* 向虚拟磁盘写入数据（以扇区为单位） */
int64_t vdisk_write(vdisk_handle_t handle, uint32_t sector, uint32_t count,
                    const void* buf);

#endif
