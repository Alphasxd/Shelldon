#include "print_working_dir.h"

int printWorkingDir(void)
{
    char *pwd = getcwd(NULL, 0);
    // 如果获取失败，打印错误信息并返回-1
    if (!pwd) {
        perror("getcwd");
        return -1;
    }

    printf("%s\n", pwd);
    free(pwd);

    return 0;
}
