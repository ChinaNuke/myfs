#ifndef __TEST__H__
#define __TEST__H__
#include "catch.hpp"

/*
 * 这个文件里是方便测试用的测试工具（test tools）
 * 编写测试程序时只需要 #include "testtools.h" 即可
 * 不需要再 #include "catch.hpp"
 */

void tt_create_diskfile(const char *filename, size_t filesize);

#endif /* __TEST__H__ */
