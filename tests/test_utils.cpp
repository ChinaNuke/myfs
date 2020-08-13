#include <cstdio>
#include <cstring>

#include "testtools.h"

extern "C" {
#include "utils.h"
}

TEST_CASE("filesize", "[utils]") {
    INFO("We are testing filesize!");

    FILE* fp = fopen("Testing/Temporary/newfile.fs", "w+");
    const char* str = "HelloWorld!";
    fwrite(str, sizeof(char), strlen(str) + 1, fp);

    REQUIRE(ut_filesize(fp) == 12);

    char out[12];
    fseek(fp, 0, SEEK_SET); /* 把文件指针重新指向开头，这一步一定要有！！
                             */
    fread(out, sizeof(char), 12, fp);

    REQUIRE(strcmp(out, str) == 0);

    fclose(fp);
    remove("Testing/Temporary/newfile.fs");
}
