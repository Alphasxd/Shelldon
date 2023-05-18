#include "word_count.h"

int wordCount(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", args[0]);
        return -1;
    }

    char buf[512];
    int char_num = 0, word_num = 0, line_num = 0;
    int i, n, in_word = 0;

    int fd;
    // open()函数用来打开参数pathname指定的文件或目录
    // 如果成功则返回文件描述符，失败则返回-1
    // O_RDONLY:只读打开
    fd = open(args[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // read()函数用来读取文件, 读取的数据存放在buf中, 读取的字节数为count
    while ((n = read(fd, buf, sizeof(buf))) > 0)
        for (i = 0; i < n; i++) {
            char_num++;
            // buf[i] == '\n'，说明是换行符，行数加1
            if (buf[i] == '\n')
                line_num++;
            // strchr()函数用来判断buf[i]是否是下面的字符
            // 如果是，说明是空格、回车、换行、制表符， 将in_word置为0
            if (strchr(" \r\t\n\v", buf[i]))
                in_word = 0;
            // 如果不是，说明是单词，将in_word置为1
            else if (!in_word) {
                word_num++; 
                in_word = 1;
            }
        }

    if (close(fd) == -1) {
        perror("close");
        return -1;
    }

    // 打印行数、单词数、字符数和文件名
    printf("line: %d  word: %d  char: %d  %s\n",
            line_num, word_num, char_num, args[1]);

    return 0;
}
