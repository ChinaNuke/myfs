#include "block.h"

uint64_t block_read(vdisk_handle_t handle, uint16_t sectors_per_block, block_handle_t no,char* buf)
{
    return  vdisk_read(handle, no * sectors_per_block, sectors_per_block, buf);
//    if(res==-1)return BLOCK_ERROR;
    
}
uint64_t block_write(vdisk_handle_t handle, uint16_t sectors_per_block, block_handle_t no,char* buf)
{
    // if (no == 0) {
    //     log_warning("写入%d号设备的0号块（可能是superblock所在块）\n", device);
    // }
    return vdisk_write(handle, no * sectors_per_block, sectors_per_block, buf);
//    if(res==-1)return BLOCK_ERROR;
}
