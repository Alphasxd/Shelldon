#include "date.h"

int date(void)
{
    // time_t是time.h中定义的一个类型，它是一个长整型数，用来存储从1970年1月1日0时0分0秒开始到现在的秒数
    char buf[32];
    time_t t = time(NULL);
    // 将time_t类型的时间转换为字符串
    ctime_r(&t, buf);
    // 输出字符串
    printf("%s", buf);
    return 0;
}
