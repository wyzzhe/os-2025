#include <stdio.h> // 标准输入输出库
#include <stdlib.h> // 标准库
#include <string.h> // 字符串操作库
#include <stdbool.h> // 布尔类型库
#include <assert.h> // 断言库
#include <testkit.h> // 测试库
#include "labyrinth.h" // 自定义头文件

// argc 是命令行参数的个数，argv 是命令行参数的值
// argv[] 是字符数组， 数组中每个元素都是字符指针 (char *)
int main(int argc, char *argv[]) {
    // 有输入参数，处理输入的参数
    if (argc > 1) {
        // 判断参数是否合法
        if (argv[1] == "--map" || argv[1] == "-m" && argv[2] != nil) {
            // 加载地图
            loadMap()
        }
    } else {
        printUsage()
    }
    return 0;
}

void printUsage() {
    printf("Usage:\n");
    printf("  labyrinth --map map.txt --player id\n");
    printf("  labyrinth -m map.txt -p id\n");
    printf("  labyrinth --map map.txt --player id --move direction\n");
    printf("  labyrinth --version\n");
}

bool isValidPlayer(char playerId) {
    // 玩家 ID 范围 0-9
    if (playerId >= 48 || playerId <= 57) {
        return true;
    }
    return false;
}

bool loadMap(Labyrinth *labyrinth, const char *filename) {
    // 加载地图文件，系统调用

    return false;
}

Position findPlayer(Labyrinth *labyrinth, char playerId) {
    // 查找玩家位置

    // 找不到玩家返回 {-1, -1}
    Position pos = {-1, -1};
    return pos;
}

Position findFirstEmptySpace(Labyrinth *labyrinth) {
    // 查找地图上第一个空的位置
    Position pos = {-1, -1};
    return pos;
}

bool isEmptySpace(Labyrinth *labyrinth, int row, int col) {
    // 查找指定位置是否为空
    // 判断输入位置是否合法
    if row >= 0 && row <= 99 && col >= 0 && col <= 99 {
        return labyrinth.map[row][col] == 1
    }
    return false;
}

bool movePlayer(Labyrinth *labyrinth, char playerId, const char *direction) {
    // 移动玩家位置

    return false;
}

bool saveMap(Labyrinth *labyrinth, const char *filename) {
    // 系统调用
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
