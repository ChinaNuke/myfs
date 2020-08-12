#ifndef __dir_entry__H__
#define __dir_entry__H__

/*目录项
 *文件名和索引节号（一一对应）的集合
 *目录项长度可变：取决于实际文件名的大小
 */

#include "inode.h"

/*文件类型表 */
#define FILE_TYPE_COUNT 8
char file_type[FILE_TYPE_COUNT][20] ={"未知","普通文件","目录","字符设备","块设备","命名管道","套接字","符号链接"};

#define MAX_NAME_LNE 255 //文件名最大长度(单位：字符)

#define DIR_ENTRY_ERROR -1

struct dir_entry{
    inode_no_t inode_addr;  //inode号
    uint8_t name_len;    //实际文件名长度
    char name[MAX_NAME_LNE];
    uint8_t file_type_id;  //文件类型id
    uint8_t rec_len;   //该目录项长度

}dir_entry_t;

#define MAX_DIR_ENTRY_COUNT 10000

dir_entry_t dir_entry_list[MAX_DIR_ENTRY_COUNT]

/*读目录项所在的blcok 
 *block:目录项存储的起始块号
*/
uint8_t read_dir_block(vdisk_handle_t handle,uint32_t blocksize, uint32_t block,
               void* buf);

/*添加目录
 *addr:当前所在目录项起始地址(块号)
 *name:要创建的文件名
*/
int add_document(char* name,vdisk_handle_t handle,uint32_t blocksize,uint32_t block);

/*删除目录 */
int del_document();

/*通过文件名查找inode号 */
inode_no_t search_inode_addr();

#endif
