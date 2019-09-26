#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "main.h"
#include "do.h"

extern int child_pid[100];
extern char *child_name[100];
extern int pid_no;
extern int foreground[100];
extern int fg_pid;
pid_t cur_pid = -2;
pid_t main_id;
size_t file_size = 256;

int main()
{

    main_id = getpid();
    // The main pid of the program
    printf("%d", main_id);
    create_shell();
    return 0;
}

void create_shell()
{
    char *home_dir = (char *)malloc(file_size * sizeof(char));  //256 is the max limit of folder_name according to Linux
    char *user_name = (char *)malloc(32 * sizeof(char));        //32 is the max limit of user_name according to Linux
    char *host_name = (char *)malloc(file_size * sizeof(char)); //256 is the max limit of host_name according to Linux
    if (getcwd(home_dir, file_size) == NULL)
    {
        perror("Err1");
    }
    if (gethostname(host_name, file_size))
    {
        perror("Err2");
    }
    if (!(user_name = getenv("USER")))
    {
        perror("Err3");
    }
    signal(SIGTSTP, control_z);
    signal(SIGCHLD, handle_child);
    signal(SIGINT, control_c);
    int no_of_commands = 0;
    while (no_of_commands > -1)
    {
        char *cur_dir = (char *)malloc(file_size * sizeof(char *));
        getcwd(cur_dir, file_size);
        char *command, *token, *display_line = (char *)malloc(file_size * sizeof(char *));
        sprintf(display_line, "\033[1;31m<%s@%s\033[0m:\033[1;36m%s\033[0m>", user_name, host_name, rel_path(cur_dir, home_dir));
        command = readline(display_line);
        // printf("%s", command);

        add_history(command);
        while ((token = strtok_r(command, ";", &command)))
        {

            if (!(strcmp(token, "quit")))
            {
                perror("Shell Terminated");
                exit(0);
                return;
            }

            if (do_it(token, home_dir) == 1)
            {

                perror("Command Does Not Exist");
            }
        }

        no_of_commands++;
    }
}
void control_c(int sig_no)
{
    // cur_pid = foreground[pid_no];
    // printf("%d\n", getpid());
    // if (getpid() != main_id)
    //     return;
    // if(cur_pid != -2) {
 
    //     kill(cur_pid, SIGINT);
    //     return ;
    // }
    
    // fprintf(stderr, "\n");
    signal(SIGINT, control_c);
    
}
void control_z(int sig_no)
{
    signal(SIGTSTP, control_z);
}
void handle_child(int sig_no)
{
    for (int i = pid_no; i >= 0; i--)
    {
        int status3;
        pid_t return_pid = waitpid(child_pid[i], &status3, WNOHANG);

        if (return_pid == 0)
        {
            /* child is still running */
            continue;
        }
        else if (return_pid == child_pid[i - 1])
        {
            printf("\nProcess %s with pid %d finished normally\n", child_name[i - 1], return_pid);
            /* child is finished */
        }
    }
    signal(SIGCHLD, handle_child);
}
char *rel_path(char *str, char *template)
{
    char *new_path = (char *)malloc(file_size * sizeof(char *));
    new_path[0] = '~';
    for (int i = strlen(template), j = 1; i <= strlen(str); i++, j++)
    {
        new_path[j] = str[i];
    }
    return new_path;
}
