#include "change_mode.h"

int changeMode(int argc, char **args)
{
    // 如果参数不够，或者第二个参数不是八进制整数，就报错
    if (argc < 3 || !isdigit(args[1][0])) {
        fprintf(stderr, "Usage: %s <mode(such as 664)> <file>\n", args[0]); 
        return -1;
    }

    // 将第二个参数转换为八进制整数
    mode_t mode = ((args[1][0]-'0')<<6) + ((args[1][1]-'0')<<3) + (args[1][2]-'0'); 

    // 改变文件权限
    int ret;
    ret = chmod(args[2], mode);

    // 如果出错，就报错
    if (ret) {
        perror("chmod");
        return -1;
    }

    return 0;
}
