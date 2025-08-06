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
#include "./parser/parser.h"

void intHandler(int sig)
{
    //write(STDOUT_FILENO, "\r\n> ", 3);
    write(STDOUT_FILENO, "\r\n> ", 4);
}

void seg_handler(int sig)
{
    disable_raw_mode();
    printf("\nseg fault\n");
    exit(1);
}

int main(void)//int argc, char *argv[])
{
    //atexit(disable_raw_mode);
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;
    //ssize_t nread;
    struct token_list *tl;
    signal(SIGSEGV, seg_handler);
    signal(SIGINT, intHandler);

    for (;;) {
        fflush(stdout);

        enable_raw_mode();
        struct list *line = readline("> ");
        disable_raw_mode();

        tl = tokenize(line); //, size);
        //printf("After tokenize\r\n");         // debug
        struct ot_node *ot = parse(tl);
        exec_tree(ot);
        free_operator_tree(ot); 
        //handle_tokens(tl);
        //free_tokens(tl);
    }

    return EXIT_SUCCESS;
}
