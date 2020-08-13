#ifndef __DIR_ENTRY__H__
#define __DIR_ENTRY__H__

#include "inode.h"
#include "virtual_disk.h"

/* 目录项
 * 表示一个文件或者一个目录，只存放文件名和索引结点号等必要的信息
 * 大小固定为 XX 字节
 * 根据目录项可以唯一确定一个文件或者目录
 */

#include "block.h"
#include "inode.h"
/*文件类型表 下标是file_type_id*/
#define FILE_TYPE_COUNT 8
char file_type[FILE_TYPE_COUNT][20] = {"未知",     "普通文件", "目录",
                                       "字符设备", "块设备",   "命名管道",
                                       "套接字",   "符号链接"};

#define MAX_NAME_LEN 29  //文件名最大长度(单位：字符)
#define DIR_ENTRY_SIZE 32
#define DIR_ENTRY_ERROR -1

typedef struct dir_entry {
    uint16_t inode_addr;  // inode号
    char name[29];
    uint8_t file_type_id;  //文件类型id

} dir_entry_t;

// #define MAX_DIR_ENTRY_COUNT 10000

// dir_entry_t dir_entry_list[MAX_DIR_ENTRY_COUNT]

/*读目录项所在的blcok
 *block:目录项存储的起始块号
 */
uint8_t read_dir_block(vdisk_handle_t handle, uint32_t blocksize,
                       uint32_t block, void* buf);

/*新建目录
 *inode_id:当前目录的inode号
 *name:要创建的文件名
 */
int add_document(char* name, vdisk_handle_t handle, uint32_t blocksize,
                 uint16_t inode_id);

/*删除目录 */
int del_document(char* name, vdisk_handle_t handle, uint32_t blocksize,
                 uint16_t inode_id);

/*通过文件名查找inode号
 *block:存放目录项的block号

 *site:该blcok在site位置可以存一个目录项
*/
int search_inode_addr(char* name, vdisk_handle_t handle, uint32_t block,
                      uint32_t blocksize, uint8_t* site);

/*读一个block,转成目录结构 */
dir_entry_t* block_to_dir(vdisk_handle_t handle, uint32_t block,
                          uint32_t blocksize);

/* 通过文件名判断文件类型，返回file_type_id*/
uint8_t name2type_no(char* name);

#endif /* __DIR_ENTRY__H__ */
