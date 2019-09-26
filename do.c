#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "pwd.h"
#include "cd.h"
#include "echo.h"
#include "history.h"
#include "ls.h"
#include "pinfo.h"
#include "nightswatch.h"
int child_pid[100];
char *child_name[100];
int pid_no = 0;
char *kshDELIM = " \r\t\n\a";
const int maxBuf = 128;
char **ksh_split_line(char *line)
{
    int bufsize = maxBuf;
    char **tokens = malloc(maxBuf* sizeof(char *));

    if (!tokens)
    {
        perror("ksh: No memory for token");
    }
    char *token;
    token = strtok(line, kshDELIM);
    int position = 0;

    while (1)
    {
        if(token == NULL)
        {
            break;
        }
        tokens[position] = token;
        token = strtok(NULL, kshDELIM);
        position++;
    }
    tokens[position] = NULL;
    return tokens;
}

int do_it(char *cmd, char *home_dir)
{

    char *token = (char *)malloc(256 * sizeof(char));

    char *cmd2 = (char *)malloc(200 * sizeof(char));
    strcpy(cmd2, cmd);

    char *command = (char *)malloc(256 * sizeof(char));
    command = strtok(cmd, " ");

    if (strcmp("pwd", command) == 0)
    {
        if (pwd(home_dir) != 0)
        {
            perror("pwd:");
        }
        return 0;
    }
    else if (strcmp("cd", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, kshDELIM);
        cd(home_dir, arguments);
        return 0;
    }
    else if (strcmp("echo", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, kshDELIM);
        echo(arguments);
        return 0;
    }
    else if (strcmp("history", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, kshDELIM);
        history(arguments);
        return 0;
    }
    else if (strcmp("ls", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, kshDELIM);
        ls(arguments);
        return 0;
    }
    else if (strcmp("pinfo", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, kshDELIM);
        pinfo(home_dir, arguments);
        return 0;
    }
    else if(strcmp("nightswatch", command)==0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, "\n");
        nightswatch(arguments);
        return 0;
    }
    else
    {
        int status, status2;
        int ampersand_flag = 0, i = 0;
        char **args = ksh_split_line(cmd2);
        for (int i = 0; args[i] != NULL; i++)
        {

            if (strcmp(args[i], "&") == 0)
                ampersand_flag = 1;
        }

        pid_t shell_pid, wait_pid, return_pid;
        shell_pid = fork();

        if (shell_pid == 0)
        {
            int a = execvp(command, args);
            if (a == -1)
            {
                perror("Command does not exist");
            }
        }
        else if (0 > shell_pid)
        {
            perror("Error forking k7_shell");
        }
        else
        {

            if (ampersand_flag == 1)
            {
                child_name[pid_no] = command;
                child_pid[pid_no++] = shell_pid;
                printf("Starting background process with id :%d\n", shell_pid);
            }
            else
            {
                do
                {
                    wait_pid = waitpid(shell_pid, &status, WUNTRACED);
                    if (!WIFEXITED(status))
                    {
                        printf("Process %s Exited %d\n", command, status);
                        fflush(stdout);
                        break;
                    }

                    if (!WIFSIGNALED(status))
                    {
                        printf("Process %s Completed %d\n", command, status);
                        fflush(stdout);
                        break;
                    }
                    if (!WIFSTOPPED(status))
                    {
                        printf("Process %s Stopped %d\n", command, status);
                        fflush(stdout);
                        break;
                    }

                } while (1);
            }
        }
        return 0;
    }

    return 1;
}