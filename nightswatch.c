#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include "do.h"
int nightswatch(char *arguments)
{
    int dirty = 0;
    int timew = 0;
    // printf("%s\n", arguments);
    char **args = ksh_split_line(arguments);
    for (int i = 0; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "dirty") == 0)
            dirty = 1;
        // printf("%s\n", args[1]);
    }
    timew = atoi(args[1]);
    // printf("%d\n", timew);

    struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI};
    char inp;
    while (dirty == 1)
    {
        FILE *fd = fopen("/proc/meminfo", "rx");
        if (fd == NULL)
        {
            perror("ksh: file open error");
            return 1;
        }
        if (poll(&mypoll, 1, 1000))
        {
            scanf("%s", &inp);
            if (inp == 'q')
                break;
        }

        int dirty_line = 17;
        char *temp_str1 = (char *)malloc(256 * sizeof(char *));
        long int size = 0;
        for (int i = 0; i < dirty_line; ++i) //Dirty present on 3rd line
            getline(&temp_str1, &size, fd);
        printf("%s", temp_str1);
        // sleep(timew);
    }
}