#include "help.h"
#include "color.h"

int help(void)
{
    
    PRINT_ATTR_BOL
	PRINT_FONT_RED
    printf("\n\n");
    printf("This is a simple shell write in C .\n\n");
    PRINT_ATTR_REC

    printf("You can use the following commands:\n\n");

    PRINT_ATTR_BOL
	PRINT_FONT_BLU
    printf("help: show help information.\n");
    printf("exit: exit the shell.\n");
    printf("pwd: show the current directory.\n");
    printf("cd: change the current directory.\n");
    printf("echo: show the parameters to standard output.\n");
    printf("cat: show the content of a file.\n");
    printf("ls: show the files in the current directory.\n");
    printf("mkdir: make a new directory.\n");
    printf("rmdir: remove a directory.\n");
    printf("rm: remove a file.\n");
    printf("date: show the current date.\n");
    printf("chmod: change the mode of a file.\n");
    printf("wc: show the number of lines, words and characters in a file.\n");
    printf("history: show the history of commands.\n");
    printf("who: show the users who are currently logged in.\n");
    printf("kill: send a signal to a process.\n");
    printf("./: run the user-compiled executable program.\n");
    printf("\n\n");
	PRINT_ATTR_REC

    return 0;
}
