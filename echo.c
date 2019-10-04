#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pwd.h"
int echo(char *str)
{
    if(str[0]=='$'){
        char * str2 = (char*)malloc(128*sizeof(char));
        for(int i=0;i<strlen(str);++i){
            str2[i] = str[i+1];
        }
        if(getenv(str2)){
        printf("%s\n", getenv(str2));
        }
        else {
            perror("Environment variable not found");
        }
        free(str2);
        return 0;
    }
    else if (strcmp(str, "~+") == 0)
    {
        if (pwd("") != 0)
        {
            perror("k7sh pwd:");
        }
        return 0;
    }
    else if(0==strcmp(str, "~")){
        printf("%s\n", getenv("HOME"));
        return 0;
    }
    else
        printf("%s\n", str);
    return 0;
}