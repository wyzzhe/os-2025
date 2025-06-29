#include <stdio.h>
#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "pstree.h"

#define HASH_TABLE_SIZE 1024

// 定义链表，存储子进程ID
typedef struct Node {
    int pid;
    struct Node* next; // 结构体未定义完成，用struct注明，引用自身
} Node;

// 定义哈希表项结构体
typedef struct HashTableItem {
    int ppid;
    Node* children;  // 当前 ppid 的子进程链表
    struct HashTableItem* next; // 下一个 HashTableItem（处理冲突）
} HashTableItem;

// 哈希表数组，存储多个哈希表结构体
HashTableItem hashTable[HASH_TABLE_SIZE];

// 哈希函数
unsigned int hash(int key) {
    return key % HASH_TABLE_SIZE;
}

// 初始化哈希表
void initHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable[i].ppid = -1;
        hashTable[i].children = NULL;
        hashTable[i].next = NULL;
    }
}

// 插入子进程到哈希表
void insertChild(int ppid, int pid) {
    unsigned int index = hash(ppid);
    // 初始化链表节点 pid
    // malloc用于数组、链表、树
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->pid = pid;
    newNode->next = NULL;

    // 初始化哈希表项 ppid
    HashTableItem* item = (HashTableItem*)malloc(sizeof(HashTableItem));
    item->ppid = ppid;
    item->children = newNode;
    item->next = NULL;

    // hashTable插槽无数据时直接插入数据
    if (hashTable[index].ppid == -1) {
        hashTable[index].ppid = ppid;
        hashTable[index].children = newNode;
    } else {
        // hashTable插槽已经有数据，开始递归遍历并判断ppid
        if (hashTable[index].ppid == ppid) {
            Node* curNode = hashTable[index].children;
            // 移动curNode到链表末尾
            while (curNode->next != NULL) {
                curNode = curNode->next;
            }
            // 尾插子进程节点
            curNode->next = newNode;
        } else {
            int flag = 0; // ppid是否存在
            HashTableItem* curItem = &hashTable[index]; // current指向当前哈希表项，当前哈希表项一定有next
            while (curItem->next != NULL) {
                // ppid 和当前哈希表项 ppid 一致则尾插子进程节点
                if (curItem->next->ppid == ppid) {
                    Node* curNode = curItem->next->children;
                    // 移动curNode到链表末尾
                    while (curNode->next != NULL) {
                        curNode = curNode->next;
                    }
                    // 尾插子进程节点
                    curNode->next = newNode;
                    flag = 1;
                } else {
                    // curItem向前移动
                    curItem = curItem->next;
                }
            }
            // 如果递归遍历完之后 ppid 还是不存在
            if (!flag) {
                curItem->next = item;
            }
        }
        




        // // ppid 已存在且等于输入的 ppid
        // if (hashTable[index].ppid == ppid) {
        //     Node* current = hashTable[index].children;
        //     while (current->next != NULL) {
        //         current = current->next;
        //     }
        //     current->next = newNode;
        // } else { // ppid 已存在但不等于输入的 ppid
        //     // 链表指针向前移动到链尾
        //     HashTableItem* current = &hashTable[index];
        //     while (current->next != NULL) {
        //         current = current->next;
        //     }
        //     // 尾插哈希表项
        //     current->next = item;
        // }
    }
}

// 打印当前哈希表项进程树
void printCurProcessTree(int ppid, int level) {
    unsigned int index = hash(ppid);
    if (hashTable[index].ppid == -1) {
        return;
    }

    // 动态生成字符串
    char indent[level * 4 + 1];
    memset(indent, '_', level * 4);
    memset(indent, ' ', (level - 1) * 4);
    indent[(level - 1) * 4] = '|';
    indent[level * 4] = '\0';

    // 递归打印当前哈希表项子进程
    Node* current = hashTable[index].children;
    while (current != NULL) {
        printf("%sPID: %d\n", indent, current->pid); // *表示可变参数
        printCurProcessTree(current->pid, level + 1);
        current = current->next;
    }
}

// 打印进程树
void printProcessTree(int ppid, int level) {
    unsigned int index = hash(ppid);
    if (hashTable[index].ppid == -1) {
        return;
    }

    // // 动态生成字符串
    // char indent[level * 4 + 1];
    // memset(indent, '_', level * 4);
    // memset(indent, ' ', (level - 1) * 4);
    // indent[(level - 1) * 4] = '|';
    // indent[level * 4] = '\0';


    // // 递归打印当前哈希表项子进程
    // Node* current = hashTable[index].children;
    // while (current != NULL) {
    //     printf("%sPID: %d\n", indent, current->pid); // *表示可变参数
    //     printProcessTree(current->pid, level + 1);
    //     current = current->next;
    // }

    // 递归打印所有哈希表项子进程
    printCurProcessTree(ppid, level);
    HashTableItem* cur = hashTable[index].next;
    while (cur != NULL) {
        printCurProcessTree(cur->ppid, level);
        cur = cur->next;
    }
}

// 打印哈希表
void printHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        // ppid 存在时递归打印进程号
        if (hashTable[i].ppid != -1) {
            printf("PPID: %d\n", hashTable[i].ppid);
            Node* current = hashTable[i].children;
            while (current != NULL) {
                printf("  Child PID: %d\n", current->pid);
                current = current->next;
            }
        }
    }
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

// 获取指定PID的PPID
int get_ppid_from_stat(const char *pid) {
    char filename[256];
    // 把格式化字符串写入字符串中
    snprintf(filename, sizeof(filename), "/proc/%s/stat", pid);

    // 打开文件
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    char stat[1024];
    // 读取文件内容至 stat
    if (fgets(stat, sizeof(stat), file) == NULL) {
        perror("fgets");
        fclose(file); // 未读取到文件内容，关闭文件
        return -1;
    }
    fclose(file); // 处理完文件，关闭文件

    int ppid;
    // 解析stat文件，获取ppid
    sscanf(stat, "%*d %*s %*c %d", &ppid); // sscanf从字符串读取数据，*表示忽略当前输入项
    return ppid;
}

bool printPids() {
    DIR *dir; // (struct __dirstream *) dir 是指向结构体的指针
    struct dirent *entry; // (struct dirent *) entry是指向结构体的指针

    // 初始化哈希表
    initHashTable();

    // 打开 /proc 目录
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir"); // fprintf(stderr)的语法糖
        return false;
    }

    // 遍历目录
    while ((entry = readdir(dir)) != NULL) {
        // 检查目录名是否为数字
        if (is_number(entry->d_name)) {
            int pid = atoi(entry->d_name);
            int ppid = get_ppid_from_stat(entry->d_name);
            if (ppid != 0) { // 当前进程有父进程
                insertChild(ppid, pid);
            }
        }
    }

    // 关闭目录
    closedir(dir);

    // 打印进程树，从根节点 (PID 1) 开始
    printf("Process Tree:\n");
    printf("PID: %d\n", 1);
    printProcessTree(1, 1);

    return true;
}

bool printChild() {
    return true;
}

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
                if (!printPids()) {
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

