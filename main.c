#include "main.h"
#include "list.h"
#include "echo.h"
#include "concatenate.h"
#include "make_dir.h"
#include "remove_dir.h"
#include "remove_file.h"
#include "change_dir.h"
#include "change_mode.h"
#include "print_working_dir.h"
#include "word_count.h"
#include "date.h"
#include "signal_kill.h"
#include "who.h"
#include "history.h"
#include "help.h"
#include "color.h"

#define FALSE 0
#define TRUE 1
#define LINEMAX 80
#define ARGMAX 16
#define BUFSIZE 512
#define HISTMAX 500


static char input[LINEMAX] = {'\0'};
static char *args[ARGMAX] = {NULL};
static int return_value = 0;    
static char *hist[HISTMAX] = {NULL};
static int hist_count = 0;
static int same_command = FALSE;

static int get_args(char *);
static void free_hist(char **);
static void free_args(char **);
static void built_in(int, char **);
static void shell_init(void);
static void format_prompt(char *, char *);
static char *str_replace(char *, char *, char *);
static int shell_clear(int, char **);

// 外部函数
int printWorkingDir(void);
int changeDir(char **);
int echo(int, char **, int);
int concatenate(int, char **);
int list(int, char **);
int makeDir(int, char **);
int removeDir(int, char **);
int removeFile(int, char **);
int date(void);
int changeMode(int, char **);
int wordCount(int, char **);
int history(char **);
int who(int, char **);
int help(void);
void shellExit(void);
int signalKill(int, char **);


int main(void)
{
    // pipe是一种IPC机制，用于进程间通信
    // pipefd[0]是读端，pipefd[1]是写端
    int pipefd[2];
    char pipech;
    // 如果创建管道失败，打印错误信息并退出
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // prompt是提示符
    char *prompt = NULL;
    char hostname[32] = {'\0'};
    gethostname(hostname, (size_t)32);

    uid_t uid;
    // 如果是root用户，提示符为#，否则为$
    prompt = (uid = getuid()) == 0 ? "# " : "$ ";   
    setuid(uid);
    setgid(getgid());

    // shell初始化
    shell_init();

    while (1) {
        
        // 忽略SIGINT、SIGQUIT和SIGTSTP信号，防止Ctrl+C、 Ctrl+\和Ctrl+Z终止程序
        if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
            fprintf(stderr, "Cannot ignore SIGINT!\n");
            exit(EXIT_FAILURE);
        }
        if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
            fprintf(stderr, "Cannot ignore SIGQUIT!\n");
            exit(EXIT_FAILURE);
        }
        if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
            fprintf(stderr, "Cannot ignore SIGTSTP!\n");
            exit(EXIT_FAILURE);
        }

        // 格式化输出prompt
        char *pwd = getcwd(NULL, 0);
        format_prompt(pwd, prompt);
        free(pwd);

        // Ctrl+D 退出程序
        int ch = fgetc(stdin);
        if (ch == EOF) {
            free_hist(hist);
            putchar(10);
            shellExit();
        }
        // 如果输入的是回车，忽略
        else
            ungetc(ch, stdin);

        // 读取输入，添加到历史记录中并解析参数
        fgets(input, LINEMAX, stdin);

        // 相同的命令不会被添加到历史记录中
        if (hist_count > 0 && strcmp(hist[hist_count - 1], input) == 0)
            same_command = TRUE;
        else
            same_command = FALSE;
        if (!same_command) {
            hist[hist_count] = malloc((strlen(input) + 1) * sizeof(char));
            if (!hist[hist_count]) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strcpy(hist[hist_count], input);
            ++ hist_count;
        }

        int argc = get_args(input);

        pid_t pid;
        if (args[0]) {
            // 如果命令不是以'-'或'.'开头，那么它是一个内置命令，否则fork并调用execvp
            if (args[0][0] != '-' && args[0][0] != '.')
                built_in(argc, args);
            // 如果fork失败，打印错误信息并退出
            else if ((pid = fork()) < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            // fork成功，子进程调用execvp
            else if (pid == 0) {
                // 关闭读端
                close(pipefd[0]);
                // 如果命令是以'-'开头，那么它是一个Bash命令
                if (args[0][0] == '-') {
                    // 如果execvp失败，打印错误信息并退出
                    if (execvp(&args[0][1], args) == -1) {
                        write(pipefd[1], "1", 1);
                        close(pipefd[1]);
                        fprintf(stderr, "bash: %s: command not found\n", &args[0][1]);
                        // 使用_exit而不是exit，因为exit会调用一些不可靠的函数
                        _exit(EXIT_FAILURE);
                    }
                    else {
                        write(pipefd[1], "0", 1);
                        close(pipefd[1]);
                    }
                }
                else if (args[0][0] == '.') {
                    if (execvp(args[0], args) == -1) {
                        write(pipefd[1], "1", 1);
                        close(pipefd[1]);
                        fprintf(stderr, "bash: %s: command not found\n", args[0]);
                        _exit(EXIT_FAILURE);
                    }
                    else {
                        write(pipefd[1], "0", 1);
                        close(pipefd[1]);
                    }
                }

                _exit(EXIT_SUCCESS);
            }
            // pid > 0 表示父进程，等待子进程结束
            else {
                close(pipefd[1]);
                read(pipefd[0], &pipech, 1);
                close(pipefd[0]);

                return_value = (pipech == '1') ? -1 : 0;

                wait(0);
            }
        }

        free_args(args);
    }

    return 0;
}

static int get_args(char *input)
{
    int i = 0, j;
    if (!input)
        return 0;

    while (1) {
        while (*input == ' ')
            input++;

        if (*input == '\n')
            break;

        if ((args[i] = malloc(LINEMAX * sizeof(char))) == NULL) {
            free_args(args);
            return 0;
        }

        j = 0;
        while (j < LINEMAX - 1 && *input != ' ' && *input != '\n')
            args[i][j++] = *input++;
        args[i][j] = '\0';

        i++;
        if (i > ARGMAX - 1) {
            free_args(args);
            return 0;
        }
    }

    return i;
}

static void free_hist(char **hist)
{
    int i;
    for (i = 0; hist[i] != NULL; i++) {
        free(hist[i]);
        hist[i] = NULL;
    }
}

static void free_args(char **args)
{
    int i;
    for (i = 0; i < ARGMAX; i++)
        if (args[i]) {
            free(args[i]);
            args[i] = NULL;
        }
}

static void built_in(int argc, char **args)
{
    if (strcmp(args[0], "help") == 0)
        return_value = help();
    else if (strcmp(args[0], "exit") == 0) {
        free_args(args);
        free_hist(hist);
        shellExit();
    }
    else if (strcmp(args[0], "pwd") == 0)
        return_value = printWorkingDir();
    else if (strcmp(args[0], "cd") == 0)
        return_value = changeDir(args);
    else if (strcmp(args[0], "echo") == 0)
        return_value = echo(argc, args, return_value);
    else if (strcmp(args[0], "cat") == 0)
        return_value = concatenate(argc, args);
    else if (strcmp(args[0], "ls") == 0)
        return_value = list(argc, args);
    else if (strcmp(args[0], "mkdir") == 0)
        return_value = makeDir(argc, args);
    else if (strcmp(args[0], "rmdir") == 0)
        return_value = removeDir(argc, args);
    else if (strcmp(args[0], "rm") == 0)
        return_value = removeFile(argc, args);
    else if (strcmp(args[0], "date") == 0)
        return_value = date();
    else if (strcmp(args[0], "chmod") == 0)
        return_value = changeMode(argc, args);
    else if (strcmp(args[0], "wc") == 0)
        return_value = wordCount(argc, args);
    else if (strcmp(args[0], "history") == 0)
        return_value = history(hist);
    else if (strcmp(args[0], "who") == 0)
        return_value = who(argc, args);
	else if (strcmp(args[0], "kill") == 0)
		return_value = signalKill(argc, args);
    else if (strcmp(args[0], "clear") == 0)
        return_value = shell_clear(argc, args);
    else {
        printf("%s: command not found...(try '-%s')\n", args[0], args[0]);
        return_value = -1;
    }
}

int shell_clear(int argc, char **args) {
    /* This prints the clear screen and move cursor to top-left corner control
    * characters for VT100 terminals. This means it will not work on
    * non-VT100 compliant terminals, namely Windows' cmd.exe, but should
    * work on anything unix-y. */
    fputs("\x1b[2J\x1b[H", stdout);
    return 0;
}

static void shell_init(void) {
    PRINT_ATTR_BOL
	PRINT_FONT_BLU
    printf("\n");
	printf("         __         ____    __\n");
	printf("   _____/ /_  ___  / / /___/ /___  ____\n");
	printf("  / ___/ __ \\/ _ \\/ / / __  / __ \\/ __ \\\n");
	printf(" (__  ) / / /  __/ / / /_/ / /_/ / / / /\n");
	printf("/____/_/ /_/\\___/_/_/\\__,_/\\____/_/ /_/\n");
    printf("\n");
	PRINT_ATTR_REC
}

static void format_prompt(char *pwd, char *prompt) {
    PRINT_ATTR_BOL
	PRINT_FONT_BLU
	printf("\n# %s", getenv("USER"));
	PRINT_ATTR_REC

    printf(" @ ");

    PRINT_ATTR_BOL
	PRINT_FONT_GRE
    printf("localhost");
    PRINT_ATTR_REC

	printf(" in ");
    
    char *oldstr = "/home/apue1452";
    char *newstr = "~";
    pwd = str_replace(pwd, oldstr, newstr);

    printf("[");

    PRINT_ATTR_BOL
	PRINT_FONT_YEL
	printf("%s", pwd);
	PRINT_ATTR_REC

    printf("]");

    PRINT_ATTR_BOL
    PRINT_FONT_RED
	printf("\n%s", prompt);
    PRINT_ATTR_REC
}


static char *str_replace(char *str, char *oldstr, char *newstr) {
    //缓冲字符串数组
    char bstr[strlen(str)];

    //memset(str,int c,n)表示把str前n个字符都转化成字符c
    memset(bstr,0,sizeof(bstr));

    int i;
    for(i = 0; i < strlen(str); i++) {
        //查找到目标字符串
        if(!strncmp(str+i,oldstr,strlen(oldstr))){
            //把新字符串贴到缓冲字符串里
            strcat(bstr,newstr);
            i += strlen(oldstr) - 1;
        }
        else {
            //如果没有找到, 将该字符写入缓冲字符串数组
            strncat(bstr,str + i,1);
        }
    }
    strcpy(str,bstr);
    return str;
}