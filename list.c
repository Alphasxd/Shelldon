#include "list.h"

#define FALSE 0
#define TRUE 1
// sizeof("rwxrwxrwx") = 10
#define STR_SIZE 10
#define FP_SPECIAL 1

char perm_str[STR_SIZE];

static char *file_perm(mode_t perm, int flags)
{
    // snprintf() 是一个安全的函数，它会检查输出缓冲区的大小，防止缓冲区溢出
    snprintf(perm_str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
            (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
            (perm & S_IXUSR) ?
                (((perm & S_ISUID) && (flags && FP_SPECIAL)) ? 's' : 'x') :
                (((perm & S_ISUID) && (flags && FP_SPECIAL)) ? 'S' : '-'),
            (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
            (perm & S_IXGRP) ?
                (((perm & S_ISGID) && (flags && FP_SPECIAL)) ? 's' : 'x') :
                (((perm & S_ISGID) && (flags && FP_SPECIAL)) ? 'S' : '-'),
            (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
            (perm & S_IXOTH) ?
                (((perm & S_ISVTX) && (flags && FP_SPECIAL)) ? 't' : 'x') :
                (((perm & S_ISVTX) && (flags && FP_SPECIAL)) ? 'T' : '-'));
    // 返回一个指向perm_str的指针
    return perm_str;
}

// 通过uid获取用户名
static char *user_name_from_id(uid_t uid)
{
    struct passwd *pwd;
    pwd = getpwuid(uid);
    return (pwd == NULL) ? NULL : pwd->pw_name;
}

// 通过gid获取组名
static char *group_name_from_id(gid_t gid)
{
    struct group *grp;
    grp = getgrgid(gid);
    return (grp == NULL) ? NULL : grp->gr_name;
}

static void display_stat(struct stat *s) 
{
    // 输出文件类型
    // 输出文件权限
    // 输出文件链接数
    // 输出文件所有者
    // 输出文件所属组
    // 输出文件大小
    // 输出文件最后修改时间
    switch (s->st_mode & S_IFMT) {
        case S_IFREG:  putchar('-');  break;
        case S_IFDIR:  putchar('d');  break; 
        case S_IFCHR:  putchar('c');  break;
        case S_IFBLK:  putchar('b');  break;
        case S_IFLNK:  putchar('l');  break;
        case S_IFIFO:  putchar('p');  break;
        case S_IFSOCK:  putchar('s');  break;
        default:  putchar('?');  break;
    }
    printf("%s ", file_perm(s->st_mode, 1));
    printf("%3ld ", (long)s->st_nlink); 
    printf("%s %s ", user_name_from_id(s->st_uid), group_name_from_id(s->st_gid));
    printf("%9lld ", (long long)s->st_size);
    printf("%.20s ", ctime(&s->st_mtime) + 4);
}

int list(int argc, char **args)
{
    struct stat path_stat;
    struct dirent ***namelist;
    int print_dirname = FALSE, has_regular = FALSE, return_error = FALSE;
    int ls_all = FALSE, ls_long = FALSE, ls_inode = FALSE, no_scan = FALSE;
    int option, i, j;
    int *sum = NULL, *error = NULL;

    // 通过while循环获取命令行参数
    while (--argc > 0 && (*++args)[0] == '-') {
        char *save_args_0 = args[0];

        while ((option = *++save_args_0))
            // -a: 显示所有文件
            // -l: 显示文件详细信息
            // -i: 显示文件inode
            // -d: 不扫描目录
            // 其他: 输出错误信息
            switch (option) {
                case 'a':  ls_all = TRUE;  break;
                case 'l':  ls_long = TRUE;  break;
                case 'i':  ls_inode = TRUE;  break;
                case 'd':  no_scan = TRUE;  break;
                default:  fprintf(stderr, "ls: illegal option %c\n"
                                "Usage: ls [-a -l -i -d] [file(s)]\n", option);
                        return -1;
            }
    }

    if (argc > 1)
        print_dirname = TRUE;

    error = calloc((argc == 0 ? 1 : argc), sizeof(int));
    if (!error) {
        perror("calloc");
        return -1;
    }

    namelist = malloc((argc == 0 ? 1 : argc) * sizeof(struct dirent **));
    if (!namelist) {
        perror("malloc");
        return -1;
    }

    sum = malloc((argc == 0 ? 1 : argc) * sizeof(int));
    if (!sum) {
        perror("malloc");
        return -1;
    }

    for (i = 0; i < (argc == 0 ? 1 : argc); i++) {
        if ((stat((argc == 0) ? "." : args[i], &path_stat)) != 0) {
            fprintf(stderr, "ls: stat: %s: No such file or directory\n", args[i]);
            error[i] = TRUE;
            return_error = TRUE;
        }

        if (S_ISDIR(path_stat.st_mode) && !no_scan) {
            sum[i] = scandir((argc == 0 ? "." : args[i]), &namelist[i], NULL, alphasort);
            if (sum[i] < 0) {
                fprintf(stderr, "ls: scandir: %s: No such file or directory\n", args[i]);
                error[i] = TRUE;
                return_error = TRUE;
            }
        }
        else {
            has_regular = TRUE;

            if (!error[i]) {
                error[i] = TRUE;

                if (ls_inode)
                    printf("%ld ", (long)path_stat.st_ino);
                if (ls_long)
                    display_stat(&path_stat);

                printf("%s\n", (argc == 0) ? "." : args[i]);
            }
        }
    }

    if (!no_scan) {
        i = 0;
        do {
            int count = 0, hidden_count = 0;

            if (!error[i]) {
                char *oldpwd = NULL;
                oldpwd = getcwd(NULL, 0);
                chdir(args[i]);

                printf((i == 0 && !has_regular) ? "" : "\n");

                if (print_dirname)
                    printf("%s:\n", args[i]);

                for (j = 0; j < sum[i]; j++) {
                    if (!ls_all && namelist[i][j]->d_name[0] == '.') {
                        hidden_count++;
                        if (namelist[i][j])
                            free(namelist[i][j]);
                        continue;
                    }

                    if ((stat(namelist[i][j]->d_name, &path_stat)) != 0) {
                        perror("stat");
                        return -1;
                    }

                    if (ls_inode)
                        printf("%ld ", (long)path_stat.st_ino);
                    if (ls_long)
                        display_stat(&path_stat);

                    count++;
                    printf("%-10s%s", namelist[i][j]->d_name,
                            ls_long ? "\n" : (count % 5 == 0 && count < sum[i] - hidden_count) ? "\n" : "  ");

                    if (namelist[i][j])
                        free(namelist[i][j]);
                }
                if (!ls_long)
                    printf("\n");

                if (namelist[i])
                    free(namelist[i]);

                chdir(oldpwd);
                free(oldpwd);
            }
        } while (++i < argc);
    }

    if (error)
        free(error);
    if (namelist)
        free(namelist);
    // 如果有错误，返回-1，否则返回0
    return return_error ? -1 : 0;
}
