#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include <grp.h>
#include <fcntl.h>
#include <time.h>

int file_properties(struct stat stat_file, char *file_name)
{
    char strings[10];
    strings[0] = (S_ISDIR(stat_file.st_mode)) ? 'd' : '-';
    strings[7] = (stat_file.st_mode & S_IROTH) ? 'r' : '-';
    strings[1] = (stat_file.st_mode & S_IRUSR) ? 'r' : '-';
    strings[4] = (stat_file.st_mode & S_IRGRP) ? 'r' : '-';
    strings[2] = (stat_file.st_mode & S_IWUSR) ? 'w' : '-';
    strings[8] = (stat_file.st_mode & S_IWOTH) ? 'w' : '-';
    strings[5] = (stat_file.st_mode & S_IWGRP) ? 'w' : '-';
    strings[3] = (stat_file.st_mode & S_IXUSR) ? 'x' : '-';
    strings[9] = (stat_file.st_mode & S_IXOTH) ? 'x' : '-';
    strings[6] = (stat_file.st_mode & S_IXGRP) ? 'x' : '-';

    printf("%s  ", strings);
    struct passwd *uname;
    uname = getpwuid(stat_file.st_uid);
    struct group *ugroup;
    ugroup = getgrgid(stat_file.st_gid);
    struct tm *time = localtime(&stat_file.st_mtime);
    char *the_time = (char *)malloc(10 * sizeof(char));
    printf("%2lu  %s", stat_file.st_nlink, uname->pw_name);
    strftime(the_time, sizeof(the_time), "%b", time);
    printf("%s  %6ld", ugroup->gr_name, stat_file.st_size);
    printf(" %s %2d  %02d:%02d", the_time, time->tm_mday, time->tm_hour, time->tm_min);

    printf(" %s\n", file_name);
}

int ls(char *arguments)
{
    struct dirent **list_of_files;
    char *cur_dir = (char *)(malloc(256 * sizeof(char)));
    getcwd(cur_dir, 256);
    struct stat dir_stat;
    int flag_a = 0, flag_l = 0;
    char *dir = (char *)(malloc(256 * sizeof(char)));
    if (arguments == NULL)
    {
        dir = ".";
    }
    else if (arguments[0] == '-')
    {
        if (arguments[1] == 'a')
        {
            flag_a = 1;
            if (arguments[2] == 'l')
            {
                flag_l = 1;
            }
        }
        else if (arguments[1] == 'l')
        {
            flag_l = 1;
            if (arguments[2] == 'a')
            {
                flag_a = 1;
            }
        }

        dir = ".";
    }
    else if (stat(arguments, &dir_stat) == 0 && S_ISDIR(dir_stat.st_mode))
    {
        dir = arguments;
    }

    int number;
    if ((number = scandir(dir, &list_of_files, NULL, alphasort)) < 0)
    {
        perror("SCANDIR");
        return 1;
    }
    int max = 0;
    for (int i = 0; i < number; i++)
    {
        if (strlen(list_of_files[i]->d_name) > max)
        {
            max = strlen(list_of_files[i]->d_name);
        }
    }

    for (int i = 0; i < number; i++)
    {
        // printf("%s\n", list_of_files[i]->d_name);
        if (flag_l == 1)
        {
            if (flag_a == 1 || (strcmp(list_of_files[i]->d_name, ".") != 0 && strcmp(list_of_files[i]->d_name, "..") != 0))
            {
                struct stat file_stat;
                if (stat(list_of_files[i]->d_name, &file_stat) == -1)
                {
                    //error
                    perror("Stat doesn't work");
                    return 1;
                }
                else
                {
                    file_properties(file_stat, list_of_files[i]->d_name);
                }
            }
        }
        else
        {
            if (flag_a == 1 || (strcmp(list_of_files[i]->d_name, ".") != 0 && strcmp(list_of_files[i]->d_name, "..") != 0))
            {
                // printf("fhao");
                if (stat(list_of_files[i]->d_name, &dir_stat) == 0 && S_ISDIR(dir_stat.st_mode))
                {
                    printf("\033[1;34m %-*s\033[0m\t", max, list_of_files[i]->d_name);
                    // printf("123\n");
                }
                else
                {
                    // printf("jjj\n");
                    printf("\033[1:32m %-*s\033[0m\t", max, list_of_files[i]->d_name);
                }
            }
        }
    }
    printf("\n");
}