#include <stdio.h>
#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "pstree.h"

int main(int argc, char *argv[]) {
    int opt;
    // 短选项
    const char *optstring = "pnV";
    // 长选项
    const struct option longopts[] = {
        {"show-pids",     no_argument,    NULL,   'p'},
        {"numeric-sort",  no_argument,    NULL,   'n'},
        {"version",       no_argument,    NULL,   'V'},
        {NULL,            0,              NULL,   0},
    };

    // 根据选项处理
    while ((opt = getopt_long (argc, argv, optstring, longopts, NULL))
            != -1) {
        switch (opt) {
            case 'p':
                if (!printPids()) {
                    return EXIT_FAILURE;
                }
                printf("打印进程号完毕\n");
                break;
            case 'n':
                if (!printChild()) {
                    return EXIT_FAILURE;
                }
                printf("pid从小到大排序完毕\n");
                break;
            case 'V':
                printf("PSTREE V0.0.0\n");
                break;
            case '?':
                printUsage();
                return EXIT_FAILURE;
            default:
                fprintf(stderr, "Unknown error\n");
                return EXIT_FAILURE;
        }
    }

    // 检查是否有额外的非选项参数
    if (optind < argc) {
        fprintf(stderr, "错误：非法的非选项参数 '%s'\n", argv[optind]);
        printUsage();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void printUsage() {
    printf("Usage:\n");
    printf("  pstree --show-pids:\n");
    printf("  pstree --numeric-sort:\n");
    printf("  pstree --version:\n");
}

// 辅助函数 检查字符串是否为数字
int is_number(const char *str) {
    while (*str) { // 逐字符遍历字符串
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++; // 移动字符串指针
    }
    return 1;
}

bool printPids() {
    DIR *dir; // (struct __dirstream *) dir 是指向结构体的指针
    struct dirent *entry; // (struct dirent *) entry是指向结构体的指针

    // 打开 /proc 目录
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir"); // fprintf(stderr)的语法糖
        return EXIT_FAILURE;
    }

    // 遍历目录
    while ((entry = readdir(dir)) != NULL) {
        // 检查目录名是否为数字
        if (is_number(entry->d_name)) {
            printf("PID: %s\n", entry->d_name);
        }
    }

    // 关闭目录
    closedir(dir);

    return true;
}

bool printChild() {
    return true;
}