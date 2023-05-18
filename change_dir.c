#include "change_dir.h"

int changeDir(char **args)
{
    static char *oldpwd = NULL;
    char *tmp = NULL;

    /*getcwd()会将当前工作目录的绝对路径复制到参数buffer所指的内存空间中,参数size为buf的空间大小
    若参数buf为NULL，getcwd()会依参数size的大小自动配置内存
    如果参数size也为0，则getcwd()会根据工作目录绝对路径的字符串长度来决定所配置的内存大小 
    */
    tmp = getcwd(NULL, 0);

    // 如果参数是"~"，就cd到$HOME
    if (args[1][0] == '~' && args[1][1] == '\0') {
        if (chdir(getenv("HOME")) == -1) {
            perror("chdir($HOME)");
            return -1;
        }
    }

    // 如果参数是"-"，就cd到$OLDPWD
    else if (args[1][0] == '-' && args[1][1] == '\0') {
        if (chdir(oldpwd) == -1) {
            perror("chdir($OLDPWD)");
            return -1;
        }
    }

    else {
        if (chdir(args[1]) == -1) {
            fprintf(stderr, "cd: %s: No such file or directory\n", args[1]);
            return -1;
        }
    }

    free(oldpwd);
    oldpwd = tmp;

    return 0;
}
