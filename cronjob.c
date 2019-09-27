#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <poll.h>
#include "do.h"
int cronjob(char *arguments, char *home_dir)
{
    char **args = ksh_split_line(arguments);
    int time_elapsed = 0;
    struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI};

    while (1)
    {
        if (poll(&mypoll, 1, 1000 * atoi(args[3])))
        {
            time_elapsed += atoi(args[3]);
            if (time_elapsed == atoi(args[5]))
                break;
        }
        built_in(args[1], home_dir); //If there is no builtin implementation.
    };
    return 0;
}