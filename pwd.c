#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "main.h"

int pwd(char* home_dir)
{
    char *pwd = (char *)malloc(256 * sizeof(char *));
    char *newd = (char *)malloc(256 * sizeof(char *));
    getcwd(pwd, 256);
    printf("%s\n", pwd);
    return 0;
}