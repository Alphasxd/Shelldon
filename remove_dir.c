#include "remove_dir.h"

int removeDir(int argc, char **args)
{
    int ret;
    // 如果参数小于2，说明没有输入目录名，打印提示信息并返回-1
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", args[0]);
        return -1;
    }
    // 删除目录
    ret = rmdir(args[1]);
    // 如果删除失败，打印错误信息并返回-1
    if (ret) {
        perror("rmdir");
        return -1;
    }

    return 0;
}
