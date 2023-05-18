#include "remove_file.h"

int removeFile(int argc, char **args)
{
    int ret;
    // 如果参数小于2，说明没有输入文件名，打印提示信息并返回-1
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", args[0]);
        return -1;
    }
    // 删除文件
    ret = unlink(args[1]);
    // 如果删除失败，打印错误信息并返回-1
    if (ret) {
        perror("unlink");
        return -1;
    }

    return 0;
}
