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
#include "env.h"
#include "jobs.h"

int child_pid[100];
char *child_name[100];
char *child_status[100];
int foreground[100];
int pid_no;
int fg_pid = 0;
extern int cur_pid;
char cur_name[100];
char *kshDELIM;
// = " \r\t\n\a";
const int maxBuf = 128;
int ampersand_flag = 0, i = 0, input = 0, output = 0, pipe_flag = 0;
int input_file = 0, output_file = 1;
char *do_command[30];
char *home_dir;
char *command;
char **ksh_split_line(char *line)
{

    int bufsize = maxBuf;
    char **tokens = malloc(maxBuf * sizeof(char *));

    if (!tokens)
    {
        perror("ksh: No memory for token");
    }
    char *token;
    token = strtok(line, kshDELIM);
    int position = 0;

    while (1)
    {
        if (token == NULL)
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
int built_in(char *cmd, char *home_dir)
{
    command = (char *)malloc(256 * sizeof(char));
    command = strtok(cmd, " ");
    char *keshDELIM = "&\r\t\n\a";
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
        arguments = strtok(NULL, keshDELIM);
        cd(home_dir, arguments);
        return 0;
    }
    else if (strcmp("echo", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, keshDELIM);
        echo(arguments);
        return 0;
    }
    else if (strcmp("history", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, keshDELIM);
        history(arguments);
        return 0;
    }
    else if (strcmp("ls", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, keshDELIM);
        ls(arguments);
        return 0;
    }
    else if (strcmp("pinfo", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, keshDELIM);
        pinfo(home_dir, arguments);
        return 0;
    }
    else if (strcmp("nightswatch", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, "\n");
        nightswatch(arguments);
        return 0;
    }

    else if (strcmp("kjob", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, "\n");
        kjob(arguments);
        return 0;
    }
    else if (strcmp("bg", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, "\n");
        bg(arguments);
        return 0;
    }
    else if (strcmp("fg", command) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, "\n");
        fg(arguments);
        return 0;
    }
    else if (strcmp("overkill", command) == 0)
    {
        overkill();
        return 0;
    }

    else
    {
        return 1;
    }
}
int parser(char *cmd)
{
    ampersand_flag = 0;
    i = 0;
    input = 0;
    output = 0;
    pipe_flag = 0;
    int pos = 0;
    kshDELIM = " \r\t\n\a";
    char **args = ksh_split_line(cmd);
    for (int i = 0; args[i] != NULL; i++)
    {

        if (strcmp(args[i], "&") == 0)
            ampersand_flag = 1;

        else if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0)
        {

            output = 1;
            if (strcmp(args[i], ">>") == 0)
            {
                output_file = open(args[++i], O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else
            {
                output_file = open(args[++i], O_WRONLY | O_TRUNC | O_CREAT, 0644);
            }
            if (0 > output_file)
            {
                perror("Output File Error");
            }
        }

        else if (strcmp(args[i], "<") == 0)
        {

            input = 1;
            input_file = open(args[++i], O_RDONLY);
            if (input_file < 0)
            {
                perror("Input File Error");
            }
        }

        else
        {
            do_command[pos] = (char *)malloc(256 * sizeof(char *));
            strcpy(do_command[pos], args[i]);
            pos++;
        }
    }
    do_command[pos] = NULL;
    return 0;
}
int k_launch(char *cmd2)
{

    int status;
    pid_t shell_pid, wait_pid, return_pid;
    shell_pid = fork(); // Fork the shell
    strcpy(cur_name, cmd2);
    cur_pid = shell_pid;
    if (shell_pid == 0) // Child Process
    {

        if (input == 1) // If there is an input file redirector (<)
        {
            dup2(input_file, 0);
            close(input_file);
            input = 0;
        }

        if (output == 1) // If there is output file redirector (> or >>)
        {
            dup2(output_file, 1);
            close(output_file);
            output = 0;
        }

        if (built_in(cmd2, home_dir) != 0) //If there is no builtin implementation.
        {

            int a = execvp(command, do_command); //Then use execvp
            if (a == -1)
            {
                perror("Command does not exist");
                exit(0);
            }
        }
        else
        {
            exit(0);
        }
    }
    else if (0 > shell_pid) // Error forking the shell
    {
        perror("Error forking k7_shell");
    }
    else // Parent process
    {

        command = strtok(cmd2, " ");
        if (ampersand_flag == 1) //Background process
        {
            kill(shell_pid, SIGTTIN);
            kill(shell_pid, SIGCONT);
            child_name[pid_no] = command;
            child_pid[pid_no++] = shell_pid;
            // printf("%d", pid_no);
            printf("Starting background process with id :%d\n", shell_pid);
        }
        else // Foreground Process
        {

            do
            {

                // cur_pid = shell_pid;
                // foreground[fg_pid++] = shell_pid;
                wait_pid = waitpid(shell_pid, &status, WUNTRACED); //wait for child process
                if (!WIFEXITED(status))
                {
                    printf("Process %s Exited %d\n", command, status);
                    child_status[wait_pid] = "Exited";
                    fflush(stdout);
                    break;
                }

                if (!WIFSIGNALED(status))
                {
                    printf("Process %s Completed %d\n", command, status);
                    child_status[wait_pid] = "Completed";
                    fflush(stdout);
                    break;
                }
                if (!WIFSTOPPED(status))
                {
                    printf("Process %s Stopped %d\n", command, status);
                    child_status[wait_pid] = "Stopped";
                    fflush(stdout);
                    break;
                }

            } while (1);
        }
    }
}
int do_it(char *cmd, char *home_dir)
{
    char *cmd2 = (char *)malloc(200 * sizeof(char));
    strcpy(cmd2, cmd);
    char *cmd3 = (char *)malloc(200 * sizeof(char));
    char *cmd4 = (char *)malloc(200 * sizeof(char));
    strcpy(cmd3, cmd);
    strcpy(cmd4, cmd);
    int pos = 0;

    kshDELIM = "|";
    char **pipe_parse = ksh_split_line(cmd);
    int pipe_fd[2];

    int inp_fd = 0;
    pid_t piped_pid;
    char *command2 = (char *)malloc(256 * sizeof(char));
  // puts(command2);
    parser(cmd4);
    kshDELIM = " \r\t\n\a";
    if (ampersand_flag == 1)
    {

        if (setpgid(0, 0) != 0)
        {
            fprintf(stderr, "Unable to start the process in background\n");
            exit(2);
        }

        k_launch(cmd2);
        return 0;
    }
    command2 = strtok(cmd3, " ");
    if (strcmp("setenv", command2) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, "\n");
        puts(arguments);
        setenvar(arguments);

        return 0;
    }
    else if (strcmp("unsetenv", command2) == 0)
    {
        char *arguments = (char *)malloc(256 * sizeof(char));
        arguments = strtok(NULL, "\n");
        unsetenvar(arguments);
        return 0;
    }

  
    if (strcmp("jobs", command2) == 0)
    {
        jobs();
        return 0;
    }
    for (int i = 0; pipe_parse[i] != NULL; i++)
    {

        strcpy(cmd2, pipe_parse[i]);
        if (pipe(pipe_fd) < 0)
        {
            exit(1);
        }
        piped_pid = fork();
        if (piped_pid == 0)
        {
            dup2(inp_fd, 0);
            if (pipe_parse[i + 1] != NULL)
            {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
            }

            parser(pipe_parse[i]);
            //Parse to set flags
            //0 is Read and 1 is Write
            k_launch(cmd2);
            exit(0);
        }

        else
        {
            wait(NULL);
            close(pipe_fd[1]);
            inp_fd = pipe_fd[0];
        }
    }
    // printf("place 3%d\n", pid_no);
    return 0;
}