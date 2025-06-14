#include <string.h>
#include "testkit.h"

UnitTest(put_me_anywhere) {
    tk_assert(114514 == 0x114514, "This will not do.");
}

UnitTest(put_me_anywhere) {
    tk_assert(NULL, "This will not do.");
}

SystemTest(test_version, ((const char *[]){ "--version" })) {
    // 检查程序退出状态
    tk_assert(result->exit_status == 0, "程序应该正常退出");

    // 检查输出内容
    tk_assert(strstr(result->output, "版本信息") != NULL, "输出应该包含版本信息");
}

int main() {}