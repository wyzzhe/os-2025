#include <stdio.h> // 标准输入输出库
#include <stdlib.h> // 标准库
#include <string.h> // 字符串操作库
#include <stdbool.h> // 布尔类型库
#include <assert.h> // 断言库
#include <testkit.h> // 测试库
#include <getopt.h> // 命令行解析库
#include "labyrinth.h" // 自定义头文件

#define MOVE 1000
#define VERSION 1001

// 函数声明
void printUsage();

// argc 是命令行参数的个数，argv 是命令行参数的值
// argv[] 是字符数组，数组中每个元素都是字符指针 (char *)
int main(int argc, char *argv[]) {
    int opt; // 命令行选项
    char playerId; // 玩家ID
    char *filename; // 地图路径
    Labyrinth labyrinth; // 迷宫地图

    // 短选项字符串
    const char *optstring = "m:p:";
    
    // 长选项数组
    const struct option longopts[] = {
        {"map",         required_argument,    NULL,   'm'},
        {"player",      required_argument,    NULL,   'p'},
        {"move",        required_argument,    NULL,   MOVE},
        {"version",     no_argument,          NULL,   VERSION},
        {NULL,          0,                    NULL,   0},
    };

    while ((opt = getopt_long (argc, argv, optstring, longopts, NULL))
            != -1) {
        switch (opt) {
            case 'm':
                // 是否要检测输入参数是否合法?
                filename = optarg;
                // 加载地图之前要先判断地图是否合法
                if (!loadMap(&labyrinth, filename)) {
                    return EXIT_FAILURE;
                }
                printf("加载地图成功\n");
                break;
            case 'p':
                // 判断玩家ID是否合法
                if (!isValidPlayer(*optarg)) {
                    return EXIT_FAILURE;
                }
                playerId = *optarg;
                break;
            case MOVE:
                // 移动玩家位置
                if (!movePlayer(&labyrinth, playerId, optarg)) {
                    return EXIT_FAILURE;
                }
                // 保存地图
                if (!saveMap(&labyrinth, filename)) {
                    return EXIT_FAILURE;
                }
                exit(0);
            case VERSION:
                printf("Labyrith Game V0.0.0\n");
                exit(0);
            case '?':
                printUsage();
                return EXIT_FAILURE;
            default:
                fprintf(stderr, "Unknown error\n");
                return EXIT_FAILURE;
        }
    }
    
    // 检查是否有额外的非选项参数（如"hello"）
    if (optind < argc) {
        fprintf(stderr, "错误：非法的非选项参数 '%s'\n", argv[optind]);
        printUsage();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void printUsage() {
    printf("Usage:\n");
    printf("  labyrinth --map map.txt --player id\n");
    printf("  labyrinth -m map.txt -p id\n");
    printf("  labyrinth --map map.txt --player id --move direction\n");
    printf("  labyrinth --version\n");
}

// 玩家 ID 范围 0-9
bool isValidPlayer(char playerId) {
    if (playerId >= 48 || playerId <= 57) {
        return true;
    }
    return false;
}

// 加载地图文件，系统调用
bool loadMap(Labyrinth *labyrinth, const char *filename) {
    if (1) {
        // 地图文件合法检测
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening file");
            return EXIT_FAILURE;
        }

        // 获取文件大小
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        rewind(file); // 重置文件指针到开头

        // 分配内存并读取全部内容
        char *content = (char*)malloc(file_size + 1); // +1 用于结尾的 '\0'
        fread(content, 1, file_size, file);
        content[file_size] = '\0'; // 添加字符串终止符

        printf("%s", content); // 输出文件内容

        free(content); // 释放内存
        fclose(file);
    }

    return false;
}

// 查找玩家位置
Position findPlayer(Labyrinth *labyrinth, char playerId) {
    Position pos = {-1, -1};
    // 查找目标字符的位置
    for (int i = 0; i < labyrinth->rows; i++) {
        for (int j = 0; j < labyrinth->cols; j++) {
            if (labyrinth->map[i][j] == playerId) {
                pos.row = i;
                pos.col = j;
            }
        }
    }
    return pos;
}

// 查找地图上第一个空的位置
Position findFirstEmptySpace(Labyrinth *labyrinth) {
    Position pos = {-1, -1};
    return pos;
}

bool isEmptySpace(Labyrinth *labyrinth, int row, int col) {
    // 查找指定位置是否为空
    // 判断输入位置是否合法
    if (row >= 0 && row <= 99 && col >= 0 && col <= 99) {
        return labyrinth->map[row][col] == 1;
    }
    return false;
}

// 移动玩家位置
bool movePlayer(Labyrinth *labyrinth, char playerId, const char *direction) {
    Position pos;
    // 先查找玩家位置
    pos = findPlayer(&labyrinth, playerId);
    if (pos.row != -1 && pos.col != -1) {
        labyrinth->map[pos.row][pos.col] = '.';
        switch (*direction) {
            case 'u':
                labyrinth->map[pos.row - 1][pos.col] = playerId;
            case 'd':
                labyrinth->map[pos.row + 1][pos.col] = playerId;
            case 'l':
                labyrinth->map[pos.row][pos.col - 1] = playerId;
            case 'r':
                labyrinth->map[pos.row][pos.col + 1] = playerId;
        }
    }
    return false;
}

bool saveMap(Labyrinth *labyrinth, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    // 逐行写入地图数据
    for (int i = 0; i < labyrinth->rows; i ++) {
        fwrite(labyrinth->map[i], sizeof(char), labyrinth->cols, file); // 写入一行
        fputc('\n', file); // 写入换行符
    }

    fclose(file);
    return false;
}

// Check if all empty spaces are connected using DFS
void dfs(Labyrinth *labyrinth, int row, int col, bool visited[MAX_ROWS][MAX_COLS]) {
    // TODO: Implement this function
}

bool isConnected(Labyrinth *labyrinth) {
    // TODO: Implement this function
    return false;
}
