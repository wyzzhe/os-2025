#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 2 // 定义最大行数
#define MAX_COLS 4 // 定义最大列数

// 条件编译
#ifdef TEST_FILEPROCESS
int main(int argc, char *argv[]) {
    char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("无法打开文件");
        return EXIT_FAILURE;
    }

    char line[MAX_COLS + 2];
    int row_count = 0;
    int col_count = 0;

    // 逐行读取文件内容
    while (fgets(line, sizeof(line), file) != NULL) {
        // 列数检查
        if (line[MAX_COLS] != '\n' && line[MAX_COLS] != '\0') {
            perror("地图列数超限");
            return EXIT_FAILURE;
        }
        row_count++; // 每读取一行，行数加1

        // 计算当前行的列数
        col_count = strlen(line);
        if (line[col_count -1] == '\n') {
            line[col_count -1] = '\0';
            col_count--; // 去掉换行符
        }

        if (row_count > MAX_ROWS) {
            perror("地图行数超限");
            return EXIT_FAILURE;
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}
#endif
