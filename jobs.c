#include <stdio.h>
#include <stdlib.h> 
#include <wait.h>
#include "do.h"
#include "main.h"
extern int child_pid[100];
extern char *child_name[100];
extern char *child_status[100];
extern int pid_no;
int jobs() {
    int status;
    printf("%d", pid_no);
    for(int i = 0; i<1; ++i) {
        
        printf("[%d]  %s  %s[%d]\n", i+1, child_status[i], child_name[i], child_pid[i]);
    }
    return 0;
}