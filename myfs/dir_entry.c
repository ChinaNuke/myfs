#include "dir_entry.h"

int add_document(char* name,vdisk_handle_t handle,uint32_t blocksize,uint32_t block)
{
    char buf[blocksize];
    int res = block_read(handle,block,blocksize,buf);
    if(res==-1)return DIR_ENTRY_ERROR;

    
}