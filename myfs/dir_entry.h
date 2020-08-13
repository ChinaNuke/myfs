#ifndef __dir_entry__H__
#define __dir_entry__H__

/*目录项
 *文件名和索引节号（一一对应）的集合
 */
#include "inode.h"
#include "block.h"
/*文件类型表 下标是file_type_id*/
#define FILE_TYPE_COUNT 8
char file_type[FILE_TYPE_COUNT][20] ={"未知","普通文件","目录","字符设备","块设备","命名管道","套接字","符号链接"};

#define DIR_ENTRY_ERROR -1

typedef struct dir_entry{
    uint16_t inode_addr;  //inode号
    char name[29];
    uint8_t file_type_id;  //文件类型id
    
}dir_entry_t;

/*node_id: 2 byte
 *name: 29 byte
 *file_type_id: 1 byte
 */
#define REC_LEN  32  /*目录项长度(单位: 字节)  1k,2k,4k block分别存32,64,128个目录项 */

/*读目录项所在的blcok 
 *block:目录项存储的起始块号
*/
// uint8_t read_dir_block(vdisk_handle_t handle,uint32_t blocksize, uint32_t block,
//                void* buf);

/*新建目录
 *inode_id:当前目录的inode号
 *name:要创建的文件名
*/
int add_document(char* name,vdisk_handle_t handle,uint32_t blocksize,uint16_t inode_id);

/*删除目录 */
int del_document(char* name,vdisk_handle_t handle,uint32_t blocksize,uint16_t inode_id);

/*通过文件名查找inode号 
 *block:存放目录项的block号
 *site:该blcok在site位置可以存一个目录项
*/
int search_inode_addr(char* name,vdisk_handle_t handle,uint32_t block,uint32_t blocksize,uint8_t *site);

/*读一个block,转成目录结构 */
dir_entry_t* block_to_dir(vdisk_handle_t handle,uint32_t block,uint32_t blocksize);

/* 通过文件名判断文件类型，返回file_type_id*/
uint8_t name2type_no(char* name);

#endif
