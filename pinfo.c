#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "main.h"

extern int main_id;
int pinfo(char *home_dir, char *arguments)
{
    int id_to_check;
    if (arguments == NULL)
    {
        id_to_check = main_id;
    }
    else
    {
        id_to_check = atoi(arguments);
    }
    char *str = (char *)malloc(128 * sizeof(char));
    char *temp_str1 = (char *)malloc(128 * sizeof(char));
    char *temp_str2 = (char *)malloc(128 * sizeof(char));
    char *exec_val = (char *)malloc(128 * sizeof(char));
    long int size = 0;
    int link1 = 0;
    sprintf(str, "/proc/%d/status", id_to_check);
    FILE *fd = fopen(str, "rx");
    if (fd == NULL)
    {
        printf("Error: while retrieving information about required process with id_to_check %d\n", id_to_check);
        return 1;
    }
    printf("The pid -- %d\n", id_to_check); //id_to_check value

    int mem_all = 15;
    int status = 3; 
    for (int i = 0; i < status; ++i) //Status present on 3rd line
        getline(&temp_str1, &size, fd);

    for (int i = 0; i < mem_all; i++) //Memory allocation
        getline(&temp_str2, &size, fd);

    sprintf(str, "/proc/%d/exe", id_to_check);
    fclose(fd);

    printf("%s", temp_str1);
    printf("(Virtual Memory)%s", temp_str2);

    link1 = readlink(str, exec_val, 500);

    if (link1 == -1)
    {
        printf("Error getting executable path pid: %d\n", id_to_check);
        fflush(stdout);
        return 1;
    }
    if (link1 != -1)
    {
        char *exec_display = (char *)malloc(512 * sizeof(char));
        exec_val[link1] = '\0';

        if (strcmp(exec_val, home_dir) < 0)
        {
            strcpy(exec_display, exec_val);
        }
        if (strcmp(exec_val, home_dir) >= 0)
        {
            int k = 1, i;
            int home_length = strlen(home_dir);
            int exec_length = strlen(exec_val);
            exec_display[0] = '~';

            for (i = home_length; i < exec_length; ++i, ++k)
            {
                exec_display[k] = exec_val[i];
            }
            //String end
            exec_display[k] = '\0';
        }

        printf("Exe path -- %s\n", exec_display);
    }

    return 0;
}