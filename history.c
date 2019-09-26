#include <stdio.h>
#include <stdlib.h>
#include <readline/history.h>
#include <readline/readline.h>

int history(char *arguments)
{
    if (arguments == NULL)
    {
        HISTORY_STATE *hist = history_get_history_state();
        HIST_ENTRY **list = history_list();
        int hist_value = hist->length;
        if (hist->length == 0)
        {
            return 0;
        }
        if (hist->length > 10)
        {
            hist_value = 10;
        }
        for (int i = hist->length - hist_value, j = 0; j < hist_value; i++, j++)
        {
            if (!list[i]->line)
            {
                return 0;
            }
            printf("%d\t%s\n", i + 1, list[i]->line);
        }
    }
    else
    {
        int req_length = atoi(arguments);
        HISTORY_STATE *hist = history_get_history_state();
        HIST_ENTRY **list = history_list();
        if (hist->length == 0)
        {
            return 0;
        }
        if (hist->length > 10)
        {
            req_length = 10;
        }
        for (int i = hist->length - req_length, j = 0; j < req_length; i++, j++)
        {
            if (!list[i]->line)
            {
                return 0;
            }
            printf("%d\t%s\n", i + 1, list[i]->line);
        }
    }
}