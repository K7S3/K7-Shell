#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include "do.h"

extern int child_pid[100];
extern char *child_name[100];

extern int pid_no;
char *child_status[100];

int jobs()
{
    int status;
    char *temp_str1 = (char *)malloc(256 * sizeof(char));
    // printf("%d\n", pid_no);
    for (int i = 0; i < pid_no; ++i)
    {
        pid_t return_pid = waitpid(child_pid[i], &status, WNOHANG);

        if (return_pid == 0)
        {
            /* still running */
            strcpy(temp_str1, "Running");
        }
        else
        {
            if (!WIFSIGNALED(status))
            {
                strcpy(temp_str1, "Exited");
            }
            if (!WIFSTOPPED(status))
            {
                strcpy(temp_str1, "Stopped");
            }
           
        }

        printf("[%d]  %s  %s[%d]\n", i + 1, temp_str1, child_name[i], child_pid[i]);
    }
    return 0;
}
int kjob(char *arguments)
{

    char **args = ksh_split_line(arguments);

    if (!args[0])
    {
        perror("kjob requires 2 arguments, none specified");
    }
    else if (!args[1])
    {
        perror("kjob requires 2 arguments, one specified");
    }
    else if (args[2])
    {
        perror("kjob requires only 2 arguments, more than 2 are given");
    }
    else
    {
        int id = atoi(args[0]);
        if(id == 0)
            perror("Cannot kill main process"); return 1;
        int signal_number = atoi(args[1]);
        kill(child_pid[id - 1], signal_number);
    }
    return 0;
}
int overkill()
{
    for (int i = 0; i < pid_no; ++i)
    {
        kill(child_pid[i], SIGKILL);
    }
    return 0;
}
int bg(char *arguments)
{
    char **args = ksh_split_line(arguments);
    if (!args[0])
    {
        perror("bg requires 1 argument");
    }
    else
    {
        int id = atoi(args[0]);
        if (id > pid_no)
        {
            perror("No backgroung Job for this value");
        }

        // kill(child_pid[id - 1], SIGTTIN); //To ensure no terminal input is not taken for a bg process
        kill(child_pid[id - 1], SIGCONT);
    }
    return 0;
}
int fg(char *arguments)
{
    char **args = ksh_split_line(arguments);
    if (!args[0])
    {
        perror("fg requires 1 argument");
    }
    else
    {
        int id = atoi(args[0]);
        if (!child_pid[id - 1])
        {
            printf("No bg process for %d", id);
            return 1;
        }
        kill(id, SIGCONT);
        waitpid(-1, NULL, WUNTRACED);
        remove_proc(id, 0);

    }
}