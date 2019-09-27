#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include "do.h"
int setenvar(char *arguments)
{

    char **args = ksh_split_line(arguments);
    puts(args[0]);
    if(args[2]){
        perror("More than 2 arguments for Setenv");
    }
    if (arguments)
    {
        if (args[1] == NULL)
            setenv(args[0], " ", 1);
        else
        {
            setenv(args[0], args[1], 1);
            puts(getenv(args[0]));
        }
    }
    else {
        perror("Setenv: No arguments");
        return 1;
    }
    return 0;
}
int unsetenvar(char *arguments) {
    char **args = ksh_split_line(arguments);
    if(arguments) {
        unsetenv(args[0]);
    }
}