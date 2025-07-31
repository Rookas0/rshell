#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../term/term.h"
#include "exec.h"

int exec_cmd(struct token_list *tl)
{
    printf("In exec_cmd\r\n");
    fflush(stdout);
    char **argv = malloc(sizeof(char *) * (tl->size + 1));
    for(int i = 0; i < tl->size; i++) {
        argv[i] = tl->tokens[i].value;
        argv[i+1] = NULL;
    }
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(pid == 0) {
        disable_raw_mode();
        if(execvp(argv[0], argv) == -1) {
            printf("Error executing %s\r\n", argv[0]);
            exit(1);
        }
    } else {
        wait(NULL);
        free(argv);
        enable_raw_mode();
    }

    return 0;
}

void exec_exit(struct token_list *tl)
{
    printf("In exit function\r\n");
    free_tokens(tl); 
    exit(0);
}
