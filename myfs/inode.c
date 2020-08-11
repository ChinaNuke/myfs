#include "inode.h"
#include "block.h"

uint32_t block_addr(vdisk_handle_t handle,inode_t* inode,uint16_t no,uint32_t blocksize)
{
    uint16_t addr_pre_block=blocksize/ADDR_SIZE;
    //0-11直接寻址
    if(no<12)return inode->direct_blocks[no];
    //一次间址
    //一个block可以存放 blocksize/addr_size 个blcok地址
    else if(no>=12 && no<(addr_pre_block+12))
    {
        char buf[blocksize];
        int res = block_read(handle,inode->single_indirect,blocksize,buf);
        if(res==-1)return INODE_ERROR;
        else return buf[no];
    }
    else if(no>=(addr_pre_block+12) && no<(addr_pre_block*addr_pre_block+addr_pre_block+12))
    {
        //先读二次间地址的直接地址块
        char buf[blocksize];
        int res = block_read(handle,inode->double_indirect,blocksize,buf);
        if(res==-1)return INODE_ERROR;

        //再读取直接地址块的第site个地址指向的block
        int site = no/addr_pre_block;
        char buf_w[blocksize];
        int res_w = block_read(handle,site,blocksize,buf_w);
        if(res_w==-1)return INODE_ERROR;
        return buf_w[no%addr_pre_block];
    }
    else
    {
        //先读三次间地址的直接地址块
        char buf[blocksize];
        int res = block_read(handle,inode->triple_indirect,blocksize,buf);
        if(res==-1)return INODE_ERROR;

        //再读取直接地址块的第site个地址指向的block
        int site = no/addr_pre_block;
        char buf_w[blocksize];
        int res_w = block_read(handle,site,blocksize,buf_w);
        if(res_w==-1)return INODE_ERROR;
        return buf_w[no%addr_pre_block];

        //再读取间接地址块的第n个地址指向的block
        int n = (no%addr_pre_block)/addr_pre_block;
        char buf_n[blocksize];
        int res_n = block_read(handle,n,blocksize,buf_n);
        if(res_n==-1)return INODE_ERROR;
        return buf_n[(no%addr_pre_block)%addr_pre_block];

    }
}
