#include "fs.h"

#include "virtual_disk.h"

int fs_format(char *filename) {
    vdisk_handle_t handle = vdisk_add(filename);
    if (handle < 0) {
        return -1;
    }
    //    myfs_format(handle, )

    vdisk_remove(handle);
}
