#include <stdio.h>
#include "mytestlib.h"

int what(int argc, char *argv[]) {
    if (argc < 2) {
        printf("%s", amazing("hello"));
    } else if (argc == 2){
        printf("%s", amazing(argv[1]));
    } else {
        printf("Argument error!\n");
    }
    return 0;
}
