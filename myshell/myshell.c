/*******************************************************************************

   @file         myshell.c

   @brief        此 shell 基于 Stephen Brennan 的 LSH (Libstephen SHell) 编写
                 目的是方便演示我们所设计的文件系统 myfs

 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "filesystem.h"

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
int mysh_create_disk(char **args);
int mysh_mount(char **args);

/*
  内建命令和它们对应的函数
 */
char *builtin_str[] = {"cd", "help", "exit", "create_disk", "mount"};

int (*builtin_func[])(char **) = {&mysh_cd, &mysh_help, &mysh_exit,
                                  &mysh_create_disk, &myfs_mount};

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
        if (chdir(args[1]) != 0) {
            perror("mysh");
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
int mysh_exit(char **args) { return 0; }

int mysh_create_disk(char **args) {
    if (args[2] == NULL) {
        fprintf(stderr,
                "mysh: \"create_disk\"命令需要两个参数！(create_disk "
                "\"myfs.fs\" 4096)\n");
    } else {
        char *buf = (char *)calloc((uint16_t)args[2], sizeof(char));
        FILE *fp = fopen(args[1], "w+b");
        fwrite(buf, sizeof(char), (uint16_t)args[2], fp);
        fclose(fp);
    }
    return 1;
}

int mysh_format() { return 0; }

int mysh_mount(char **args) { return 0; }

/**
  @brief 启动一个程序，然后等待它执行终止。
  @param args 以 Null 结尾的参数列表(包括程序名).
  @return Always returns 1, to continue execution.
 */
int mysh_launch(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // 当前进程是子进程
        if (execvp(args[0], args) == -1) {
            perror("mysh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // fork 发生错误
        perror("mysh");
    } else {
        // 当前进程是父进程
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

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
