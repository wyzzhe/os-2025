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
#define MAX_ROWS 100
#define MAX_COLS 100

// 函数声明
void printUsage();

// 定义方向数组，用于表示上下左右四个方向
int dr[] = {-1, 1, 0, 0}; // 行位移
int dc[] = {0, 0, -1, 1}; // 列位移

// argc 是命令行参数的个数，argv 是命令行参数的值
// argv[] 是字符数组，数组中每个元素都是字符指针 (char *)
int main(int argc, char *argv[]) {
    int opt; // 命令行选项
    char playerId = 0; // 玩家ID
    char *filename = NULL; // 地图路径
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
                break;
            case VERSION:
                printf("Labyrinth Game V0.0.0\n");
                break;
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
    // 打开文件
    FILE *file = fopen(filename, "r");
    if (file == NULL) { // 文件不存在
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // 初始化迷宫结构体
    labyrinth->rows = 0;
    labyrinth->cols = 0;

    // 逐行读取文件内容
    char line[MAX_COLS + 2]; // MAX_COLS + 2 为 fgets() 行尾的 '\n' 和 '\0' 提供位置
    int row_count = 0;
    int col_count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        // 列数检查
        if (line[MAX_COLS] != '\n' && line[MAX_COLS] != '\0') {
            fprintf(stderr, "Error: 地图列数超限.\n");
            fclose(file);
            return EXIT_FAILURE;
        }
        row_count++; // 每读取一行，行数加1

        // 计算当前行的列数
        col_count = strlen(line);
        if (line[col_count - 1] == '\n') {
            line[col_count - 1] == '\0'; // 去掉换行符
            col_count--; // 当前行列数更新为去掉换行符的列数
        }

        // 行数检查
        if (row_count > MAX_ROWS) {
            fprintf(stderr, "Error: 地图行数超限.\n");
            fclose(file);
            return EXIT_FAILURE;
        }

        // 将当前行内容复制到 labyrinth 的 map 中
        strcpy(labyrinth->map[labyrinth->rows], line);
    }
   
    // 检查是否成功读取了地图
    if (labyrinth->rows == 0 || labyrinth->cols == 0) {
        fprintf(stderr, "Error: Failed to load map.\n");
        fclose(file);
        return false;
    }

    // 地图合法检测
    if (!isConnected(labyrinth)) {
        fprintf(stderr, "Error: The map is not connected.\n");
        fclose(file);
        return false;
    }

    // 逐行打印地图
    printf("Map loaded successfully:\n");
    for (int i = 0; i < labyrinth->rows; i++) {
        printf("%s\n", labyrinth->map[i]);
    }
    return true;
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
    // 找到第一个空闲空间作为起点
    int startRow = -1, startCol = -1;
    for (int i = 0; i < labyrinth->rows; i++) {
        for (int j = 0; j < labyrinth->cols; j++) {
            if (labyrinth->map[i][j] == '.') {
                startRow = i;
                startCol = j;
                break;
            }
        }
        if (startRow != -1) {
            break;
        }
    }
    
    // 如果没有找到空闲空间，直接返回 false
    if (startRow == -1) {
        return pos;
    }

    pos.row = startRow;
    pos.col = startCol;

    return pos;
}

bool isEmptySpace(Labyrinth *labyrinth, int row, int col) {
    if (row >= 0 && row <= 99 && col >= 0 && col <= 99) {
        return labyrinth->map[row][col] == 1;
    }
    return false;
}

// 移动玩家位置
bool movePlayer(Labyrinth *labyrinth, char playerId, const char *direction) {
    Position pos;
    // 先查找玩家位置
    pos = findPlayer(labyrinth, playerId);
    if (pos.row != -1 && pos.col != -1) {
        labyrinth->map[pos.row][pos.col] = '.'; // 清楚当前玩家位置

        // 根据方向移动玩家
        if (strcmp(direction, "up") == 0) {
            if (pos.row > 0 && labyrinth->map[pos.row - 1][pos.col] == '.') {
                labyrinth->map[pos.row - 1][pos.col] = playerId;
            }
        } else if (strcmp(direction, "down") == 0) {
            if (pos.row < labyrinth->rows && labyrinth->map[pos.row + 1][pos.col] != '.') {
                labyrinth->map[pos.row + 1][pos.col] = playerId;
            }
        } else if (strcmp(direction, "left") == 0) {
            if (pos.col > 0 && labyrinth->map[pos.row][pos.col - 1] != '.') {
                labyrinth->map[pos.row][pos.col - 1] = playerId;
            }
        } else if (strcmp(direction, "right") == 0) {
            if (pos.col < labyrinth->cols && labyrinth->map[pos.row][pos.col - 1] != '.') {
                labyrinth->map[pos.row][pos.col + 1] = playerId;
            }
        } else {
            printf("未知方向：%s\n", direction);
            return false;
        }

        return true; // 移动成功
    }

    return false; // 玩家未找到或移动失败
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
    // 检查是否越界 或 是否是墙 或 是否已访问
    if (row < 0 || row >= labyrinth->rows || col < 0 || col >= labyrinth->cols ||
    labyrinth->map[row][col] == '#' || labyrinth->map[row][col] >= 0 ||
    labyrinth->map[row][col] <= 9 || visited[row][col]) {
        return; // 如果找到终点，直接返回
    }

    // 标记当前节点为已访问
    visited[row][col] = true;
    
    // 递归探索四个方向
    dfs(labyrinth, row + 1, col, visited); //向下
    dfs(labyrinth, row - 1, col, visited); //向上
    dfs(labyrinth, row, col + 1, visited); //向右
    dfs(labyrinth, row, col - 1, visited); //向左
}

bool isConnected(Labyrinth *labyrinth) {
    // 初始化访问标记数组
    bool visited[MAX_ROWS][MAX_COLS] = {false};

    Position pos = findFirstEmptySpace(labyrinth);

    // 从起点开始 DFS　遍历
    dfs(labyrinth, pos.row, pos.col, visited);

    // 检查是否所有空闲空间都被访问过
    for (int i = 0; i < labyrinth->rows; i++) {
        for (int j = 0; j < labyrinth->cols; j++) {
            if (labyrinth->map[i][j] == '.' && !visited[i][j]) {
                return false; // 找到未访问的空闲空间，说明此迷宫不连通
            }
        }
    }

    return true; // 所有空闲空间都已访问，说明连通
}
