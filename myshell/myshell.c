/*******************************************************************************

   @file         myshell.c

   @brief        此 shell 基于 Stephen Brennan 的 LSH (Libstephen SHell) 编写
                 目的是方便演示我们所设计的文件系统 myfs

 *******************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "filesystem.h"
#include "fs.h"

/*
 * TODO(peng):
 * 计划实现以下命令
 * mount/unmount
 * ls
 * cd
 * pwd
 * mkdir/rmdir
 * touch
 * mv
 * link/unlink
 * chmod
 *
 */

/*
  内建 shell 命令的函数声明:
 */
int mysh_cd(char **args);
int mysh_help(char **args);
int mysh_exit(char **args);
int mysh_createdisk(char **args);
int mysh_format(char **args);
int mysh_mount(char **args);
int mysh_unmount(char **args);
int mysh_pwd(char **args);
int mysh_df(char **args);
int mysh_ls(char **args);
int mysh_chdisk(char **args);
int mysh_mkdir(char **args);
int mysh_touch(char **args);
int mysh_rm(char **args);
int mysh_stat(char **args);
int mysh_ln(char **args);

/*
  内建命令和它们对应的函数
 */
char *builtin_str[] = {
    "cd", "help", "exit",   "createdisk", "format", "mount", "unmount", "pwd",
    "df", "ls",   "chdisk", "mkdir",      "touch",  "rm",    "stat",    "ln"};

int (*builtin_func[])(char **) = {
    &mysh_cd,     &mysh_help,  &mysh_exit,    &mysh_createdisk,
    &mysh_format, &mysh_mount, &mysh_unmount, &mysh_pwd,
    &mysh_df,     &mysh_ls,    &mysh_chdisk,  &mysh_mkdir,
    &mysh_touch,  &mysh_rm,    &mysh_stat,    &mysh_ln};

int mysh_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

/*
  内建命令的实现
*/

/**
   @brief 内建命令: 切换目录
   @param args 参数列表.  args[0] 为 "cd".  args[1] 是要切换到的目录
   @return Always returns 1, to continue executing.
 */
int mysh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"cd\"命令需要一个参数！\n");
    } else {
        if (fs_cd(args[1]) != 0) {
            fprintf(stderr, "mysh: 切换目录失败，请检查该目录是否存在！\n");
        }
    }
    return 1;
}

/**
   @brief 内建命令: 打印帮助信息
   @param args 参数列表.
   @return Always returns 1, to continue executing.
 */
int mysh_help(char **args) {
    printf("欢迎使用 My Shell!\n");
    printf("你可以使用下面的内建命令:\n");

    for (int i = 0; i < mysh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

/**
   @brief 内建命令: 退出 shell.
   @param args 参数列表.
   @return Always returns 0, to terminate execution.
 */
int mysh_exit(char **args) {
    for (char c = 'A'; c <= 'Z'; c++) {
        fs_unmount(c);
    }
    return 0;
}

int mysh_createdisk(char **args) {
    if (args[2] == NULL) {
        fprintf(stderr,
                "mysh: \"createdisk\"命令需要两个参数！(createdisk "
                "myfs.fs 16(MBytes))\n");
    } else {
        if (access(args[1], F_OK) != -1) {
            fprintf(stderr, "mysh: 虚拟磁盘%s已存在，请勿重复创建！\n",
                    args[1]);
            return 1;
        }
        char *buf = (char *)calloc(atoi(args[2]) * 1024 * 1024, sizeof(char));
        FILE *fp = fopen(args[1], "w+b");
        fwrite(buf, sizeof(char), atoi(args[2]) * 1024 * 1024, fp);
        fclose(fp);
    }
    return 1;
}

int mysh_format(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr,
                "mysh: \"format\"命令需要两个参数！(format myfs.fs 4096)\n");
    } else {
        if (fs_format(args[1], atoi(args[2])) != 0) {
            fprintf(stderr, "mysh: 格式化虚拟磁盘%s失败！\n", args[1]);
        } else {
            fprintf(stdout, "mysh: 格式化虚拟磁盘%s成功！\n", args[1]);
        }
    }
    return 1;
}

int mysh_mount(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"mount\"命令需要一个参数！(mount myfs.fs)\n");
    } else {
        int ret = fs_mount(args[1]);
        if (ret == -1) {
            fprintf(stderr, "mysh: 挂载虚拟磁盘%s失败！请检查是否已经挂载过.\n",
                    args[1]);
        } else {
            fprintf(stdout, "mysh: 挂载虚拟磁盘%s成功！盘符为%c.\n", args[1],
                    ret);
        }
    }
    return 1;
}

int mysh_unmount(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"unmount\"命令需要一个参数！(unmount A)\n");
    } else {
        int ret = fs_unmount(args[1][0]);
        if (ret == -1) {
            fprintf(stderr, "mysh: 卸载虚拟磁盘%s失败！请检查是否已挂载该盘.\n",
                    args[1]);
        } else {
            fprintf(stdout, "mysh: 卸载虚拟磁盘%s成功！\n", args[1]);
        }
    }
    return 1;
}

int mysh_pwd(char **args) {
    if (args[1] != NULL) {
        fprintf(stderr, "mysh: \"pwd\"命令不需要参数！\n");
    } else {
        char *ret = fs_pwd();
        fprintf(stdout, "mysh: 当前路径为：%s\n", ret);
    }
    return 1;
}

int mysh_df(char **args) {
    if (args[1] != NULL) {
        fprintf(stderr, "mysh: \"disks\"命令不需要参数！\n");
    } else {
        fprintf(stdout, "mysh: 当前已挂载的磁盘有：\n");
        fs_df();
    }
    return 1;
}

int mysh_ls(char **args) {
    if (args[1] != NULL) {
        fprintf(stderr, "mysh: \"ls\"命令不需要参数！\n");
    } else {
        fs_ls();
    }
    return 1;
}

int mysh_chdisk(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"chdisk\"命令需要一个参数！(chdisk A)\n");
    } else {
        if (fs_chdisk(args[1][0]) != 0) {
            fprintf(stderr, "mysh: 切换磁盘失败，请检查该盘符是否存在！\n");
        }
    }
    return 1;
}

int mysh_mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"mkdir\"命令需要一个参数！(mkdir mydir)\n");
    } else {
        if (fs_mkdir(args[1]) != 0) {
            fprintf(stderr, "mysh: 创建目录%s失败！\n", args[1]);
        } else {
            fprintf(stderr, "mysh: 创建目录%s成功！\n", args[1]);
        }
    }
    return 1;
}

int mysh_touch(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"touch\"命令需要一个参数！(touch myfile)\n");
    } else {
        if (fs_touch(args[1]) != 0) {
            fprintf(stderr, "mysh: 创建文件%s失败！\n", args[1]);
        } else {
            fprintf(stderr, "mysh: 创建文件%s成功！\n", args[1]);
        }
    }
    return 1;
}

int mysh_rm(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"rm\"命令需要一个参数！(rm myfile)\n");
    } else {
        if (fs_rm(args[1]) != 0) {
            fprintf(stderr, "mysh: 删除文件或目录%s失败！\n", args[1]);
        } else {
            fprintf(stdout, "mysh: 删除文件或目录%s成功！\n", args[1]);
        }
    }
    return 1;
}

int mysh_stat(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mysh: \"stat\"命令需要一个参数！(stat myfile)\n");
    } else {
        if (fs_stat(args[1]) != 0) {
            fprintf(stderr, "mysh: 删除文件或目录%s失败！\n", args[1]);
        }
    }
    return 1;
}

int mysh_ln(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "mysh: \"ln\"命令需要两个参数！(ln mylink target)\n");
    } else {
        if (fs_link(args[1], args[2]) != 0) {
            fprintf(stderr, "mysh: 创建目录%s失败！\n", args[1]);
        } else {
            fprintf(stderr, "mysh: 创建目录%s成功！\n", args[1]);
        }
    }
    return 1;
}

/**
  @brief 启动一个程序，然后等待它执行终止。
  @param args 以 Null 结尾的参数列表(包括程序名).
  @return Always returns 1, to continue execution.
 */
int mysh_launch(char **args) { return 1; }

/**
   @brief 执行内建命令或者启动程序
   @param args 以 Null 结尾的参数列表(包括程序名).
   @return 1 表示 shell 应继续运行, 0 表示 shell 应退出
 */
int mysh_execute(char **args) {
    if (args[0] == NULL) {
        // 输入空命令
        return 1;
    }

    for (int i = 0; i < mysh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return mysh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief 从标准输入读入一行
   @return 读入的行
 */
char *mysh_read_line(void) {
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "mysh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief 将一行字符串分割成 tokens.
   @param line 一行字符串
   @return 以 Null 结尾的 token 数组
 */
char **mysh_split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token, **tokens_backup;

    if (!tokens) {
        fprintf(stderr, "mysh: 内存分配错误！\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "mysh: 内存分配错误！");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/**
   @brief shell 主循环.
 */
void mysh_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("%s", fs_pwd());
        printf("> ");
        line = mysh_read_line();
        args = mysh_split_line(line);
        status = mysh_execute(args);

        free(line);
        free(args);
    } while (status);
}

/**
   @brief shell 入口
 */
int main(int argc, char **argv) {
    // Load config files, if any.

    mysh_help(NULL);
    // Run command loop.
    mysh_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}
