#include "concatenate.h"

#define FALSE 0
#define TRUE 1
#define BUFSIZE 512

int concatenate(int argc, char **args)
{
    char buf[BUFSIZE];
    int fd, ret;
    int return_error = FALSE;

    // 如果没有参数，就从标准输入读取数据，然后输出到标准输出
    if (argc == 1) {
        fd = 0;
        // read()会从文件描述符fd所指的文件中读取count个字节到buf中
        while ((ret = read(fd, buf, sizeof(buf))) > 0)
            if (write(1, buf, ret) < 0) {
                perror("write");
                return -1;
            }
    }
    // 如果有参数，就从文件读取数据，然后输出到标准输出
    else {
        while (argc-- > 1) {
            fd = open(*++args, O_RDONLY);
            // 如果文件打开失败，就报错
            if (fd == -1) {
                fprintf(stderr, "cat: %s: No such file or directory\n", *args);
                return_error = TRUE;
                continue;
            }
            // 从文件读取数据，然后输出到标准输出
            while ((ret = read(fd, buf, sizeof(buf))) > 0)
                if (write(1, buf, ret) < 0) {
                    perror("write");
                    return -1;
                }
            // 如果文件关闭失败，就报错
            if (close(fd) == -1) {
                perror("close");
                return -1;
            }
        }
    }
    // 如果有错误，就返回-1，否则返回0
    return return_error ? -1 : 0;
}
