#include "make_dir.h"

int makeDir(int argc, char **args)
{
    int ret;
    if (argc < 2) {
        // 如果参数小于2，说明没有输入目录名，打印提示信息并返回-1
        fprintf(stderr, "Usage: %s <directory>\n", args[0]);
        return -1;
    }
    // 创建目录，权限为0775
    ret = mkdir(args[1], 0775);
    // 如果创建失败，打印错误信息并返回-1
    if (ret) {
        perror("mkdir");
        return -1;
    }

    return 0;
}
