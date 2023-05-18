#include "who.h"

int who(int argc, char **args)
{
    struct utmp *ut;
    time_t t;

    if (argc > 1)
        // utmpname()函数用来设置utmp文件的文件名，如果utmpname()函数执行成功则返回0，否则返回-1
        if (utmpname(args[1]) == -1) { 
            perror("utmpname");
            return -1;
        }
    
    setutent();

    printf("USER       PID LINE   HOST            TIME\n");
    // 如果utmp文件中有数据，则返回utmp结构体指针，否则返回NULL
    while ((ut = getutent()) != NULL)
        // 如果ut_type为7，说明是用户登录类型
        if (ut->ut_type == 7) {
            printf("%-8s ", ut->ut_user);
            printf("%5ld %-6.6s %-15.15s ", (long) ut->ut_pid, ut->ut_line, ut->ut_host);
            t = ut->ut_tv.tv_sec;
            printf("%s", ctime(&t));
        }

    endutent();

    return 0;
}
