#include "history.h"

// **hist是一个二级指针，指向一个指针数组，每个指针指向一个字符串
int history(char **hist) 
{
    int i;
    // 输出历史命令
    for (i = 0; hist[i] != NULL; i++)
        // %-5d表示输出一个整数，占5个字符宽度，左对齐
        printf("%-5d %s", i, hist[i]);

    return 0;
}
