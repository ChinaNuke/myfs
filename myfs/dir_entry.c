#include "dir_entry.h"

uint16_t search_inode_addr(char* name,vdisk_handle_t handle,uint32_t block,uint32_t blocksize,)
{
    void *buf = calloc(1, blocksize);
    int res = block_read(handle, blocksize, block, buf);
    if(res==-1)return DIR_ENTRY_ERROR;

    int dir_num = blocksize/REC_LEN;  /*一个块的目录项数目 */
    /*将一个目录blcok拆分成多个目录项 */
    dir_entry_t dir[dir_num];
    for(int i=0;i<dir_num,i++){
        memcpy(dir[i], buf+(i*REC_LEN), sizeof(dir_entry_t));
    }
    free(buf);
    /*通过文件名查找indoe号 */
    for(int i=0;i<dir_num;i++){
        if(strcmp(dir[i]->name,name)==0){
            return dir[i]->inode_addr;
        }
    }
    /* 没找到返回-1*/
    return DIR_ENTRY_ERROR; 
}


int add_document(char* name,vdisk_handle_t handle,uint32_t blocksize,uint16_t inode_id)
{
    /*输入indoe号，从磁盘读出inode,从indoe的地址读出blcok，转成目录项数组，添加目录项，（写回） */
    /*
     *inode.h规定inode是128字节，一个block有 blocksize/128 个inode
     *已知inode_id,读取的block_id是 inode_id%(blocksize/128)
     */
    uint32_t block_id = inode_id%(blocksize/128);

    void *buf = calloc(1, blocksize);
    int res = block_read(handle, blocksize, block_id, buf);
    if(res==-1)return DIR_ENTRY_ERROR;
    /*取出一个blcok中的某一个inode */
    inode_t inode;
    memcpy(inode, buf+(inode_id-block_id)*128, sizeof(inode_t));
    free(buf);
    /*取inode地址指向的block(目录项集合) 
     *可能分布在几个块（当前文件夹中有很多文件）
     *就得找到一个有空位置的block
    */

}


