#ifndef __DIR_ENTRY__H__
#define __DIR_ENTRY__H__

#include "data_block.h"
#include "inode.h"
#include "super_block.h"
#include "virtual_disk.h"

/* 目录项
 * 表示一个文件或者一个目录，只存放文件名和索引结点号等必要的信息
 * 大小固定为 XX 字节
 * 根据目录项可以唯一确定一个文件或者目录
 */

#define MAX_NAME_LEN 29  //文件名最大长度(单位：字符)
#define DIR_ENTRY_SIZE 32
#define DIR_ENTRY_ERROR -1

/* FTYPE_UNUSED 必须为0，其他无所谓 */
enum filetypes { FTYPE_UNUSED = 0, FTYPE_FILE, FTYPE_DIR };

typedef struct dir_entry {
    char name[29];
    uint16_t inode;     // inode号
    uint8_t file_type;  // 文件类型id

} __attribute__((packed)) dir_entry_t;

int create_dentry(vdisk_handle_t handle, super_block_t* sb, uint8_t* bitmap,
                  dir_entry_t* parent, char* name, uint8_t file_type);

int remove_dentry(vdisk_handle_t handle, super_block_t* sb, uint8_t* bitmap,
                  dir_entry_t* dentry, dir_entry_t* parent);

int create_link(vdisk_handle_t handle, super_block_t* sb, dir_entry_t* parent,
                char* name, dir_entry_t* target);

#endif /* __DIR_ENTRY__H__ */
