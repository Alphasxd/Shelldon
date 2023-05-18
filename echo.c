#include "echo.h"

int echo(int argc, char **args, int return_value)
{
    // args[0]是"echo"，args[1]是参数
    // 如果参数是"$?"，就输出上一个命令的返回值
    if (strcmp(args[1], "$?") == 0) {
        printf("%d\n", return_value);
    }
    else {
        // 输出参数
        while (--argc) {
            printf("%s", *++args);
            // 如果不是最后一个参数，就输出空格
            printf(argc > 0 ? " " : "");
        }
        putchar('\n');
    }
    return 0;
}
