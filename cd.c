#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int cd(char *home_directory, char *new_directory)
{
    if(new_directory == NULL){
        chdir(home_directory);
        // getcwd(current_directory, 256);
        return 0;
    }
    // printf("%s\n%s\n", home_directory, new_directory);
    char *current_directory = (char *)(malloc(256 * sizeof(char)));
    if ((strcmp(new_directory, ".")) == 0)
    {
        return 0;
    }
    else if ((strcmp(new_directory, "..")) == 0)
    {
        if (strcmp(getcwd(current_directory, 256), home_directory) != 0)
        {
            for (int i = strlen(current_directory); i >= strlen(home_directory); i--)
            {
                if (current_directory[i] == '/')
                {
                    chdir("..");
                    break;
                }
            }
        }
        else
        {
            return 0;
        }
    }
   
    else if (strcmp(new_directory, "~") == 0)
    {
        getcwd(current_directory, 256);
        chdir(home_directory);
        return 0;
    }
    else
    {
        if (chdir(new_directory))
        {
            perror("Directory Change ");
        }
        else{
            getcwd(current_directory, 256);

        }
    }
}