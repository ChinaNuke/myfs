#include "file_operation.h"

size_t u_filesize(FILE *fp)
{
    size_t old = ftell(fp);
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, old, SEEK_SET);
    return size;
}
