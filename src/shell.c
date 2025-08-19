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
    // On interupt( Ctrl C) reprint prompt without exiting
    write(STDOUT_FILENO, "\r\n> ", 4);
    exit(0);
}

void seg_handler(int sig)
{
    // On segfault disable raw mode before exiting
    disable_raw_mode();
    printf("\nseg fault\n");
    exit(1);
}

int main(void)
{
    // Ensure raw mode is disabled on program exit
    atexit(disable_raw_mode);

    char *line = NULL;
    size_t size = 0;
    ssize_t nread;
    struct token_list *tl;
    signal(SIGSEGV, seg_handler);
    signal(SIGINT, intHandler);

    for (;;) {
        fflush(stdout);

        // Raw mode for input, not for running commands.
        enable_raw_mode();
        struct line *ln = readline("> ");
        disable_raw_mode();
        free_line(ln);
        continue;
        //tl = tokenize(line);
        if(tl->size == 0) {
            continue;
        }

        // After making tree from tokens, tokens are no longer needed
        struct ot_node *ot = parse(tl);
        free_token_list(tl);

        exec_tree(ot);
        free_operator_tree(ot); 
    }

    return EXIT_SUCCESS;
}
