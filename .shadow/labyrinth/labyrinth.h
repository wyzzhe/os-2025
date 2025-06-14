#include <stdbool.h>

#define MAX_ROWS 100 // 编译前文本替换, MAX_ROWS 不占用任何内存
#define MAX_COLS 100
#define VERSION_INFO "Labyrinth Game"

typedef struct {
    char map[MAX_ROWS][MAX_COLS]; // map 是二维数组
    int rows;
    int cols;
} Labyrinth;

typedef struct {
    int row;
    int col;
} Position;

// 函数声明，相当于接口，在 labyrinth.c 中实现这些函数，其他模块可以调用这些函数
bool isValidPlayer(char playerId);
bool loadMap(Labyrinth *labyrinth, const char *filename);
Position findPlayer(Labyrinth *labyrinth, char playerId);
Position findFirstEmptySpace(Labyrinth *labyrinth);
bool isEmptySpace(Labyrinth *labyrinth, int row, int col);
// const 编译时处理，为 direction 常量分配内存
// labyrinth 的类型是 Labyrinth *，direction 的类型是 const char *，const 表示 direction 的值不能被函数修改
bool movePlayer(Labyrinth *labyrinth, char playerId, const char *direction);
bool saveMap(Labyrinth *labyrinth, const char *filename);
bool isConnected(Labyrinth *labyrinth);
