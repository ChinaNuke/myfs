#define CATCH_CONFIG_MAIN

#include "testtools.h"

/* tt_ 表示 test tools */
void tt_create_diskfile(const char *filename, size_t filesize) {
    char *buf = (char *)calloc(filesize, sizeof(char));
    FILE *fp = fopen(filename, "w+b");
    fwrite(buf, sizeof(char), filesize, fp);
    fclose(fp);
    free(buf);
}
