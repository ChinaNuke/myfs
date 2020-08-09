/* 占个位置，防止报错 */

#include <string.h>
#include <stdlib.h>
#include "mytestlib.h"

/*
 * 函数功能为：
 * 传入一个字符串，在字符串末尾添加叹号!和换行，返回新的字符串
 *
 */
char * amazing(char * str) {
    int len = strlen(str) + 2;
    char * res = (char *) malloc(len * sizeof(char));
    strcpy(res, str);
    strcat(res, "!\n");
    return res;
}
