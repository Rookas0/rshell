#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#include <stdbool.h>

#include "./term/term.h"
#include "./list/list.h"
#include "./tokenize/tokenize.h"
#include "./readline/readline.h"
#include "./exec/exec.h"
#include "./exec/builtins/builtins.h"


int handle_tokens(struct token_list *tl) {
    char * cmd = NULL;
    cmd = tl->tokens[0].value;
    if(strcmp(cmd, "cd") == 0) {
        cd(tl->tokens[1].value);
        //printf("built-ins\r\n");
    } else if(strcmp(cmd, "exit") == 0) {
        exec_exit(tl);
        printf("Why are we here...\r\n");
    }
    else {
        printf("Exec else\r\n");
        exec_cmd(tl);
    }
    return 0;
}


void intHandler(int sig) { 
    //write(STDOUT_FILENO, "\r\n> ", 3);
    write(STDOUT_FILENO, "\r\n> ", 4);
}

void seg_handler(int sig) {
    disable_raw_mode();
    printf("\nseg fault\n");
    exit(1);
}

int main(void)//int argc, char *argv[])
{
    enable_raw_mode();
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;
    //ssize_t nread;
    struct token_list *tl;
    signal(SIGSEGV, seg_handler);
    signal(SIGINT, intHandler);
    for(;;) {
        fflush(stdout);
        struct list *line = readline("> ");
        tl = tokenize(line); //, size);
        printf("After tokenize\r\n");
        handle_tokens(tl);
        free_tokens(tl);
    }
    return EXIT_SUCCESS;
}
