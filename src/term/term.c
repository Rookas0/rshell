/*** termios ***/ 
#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

#include "term.h"
struct termios orig_termios;
static bool in_raw_mode = false;

/*
*  Saves original termios attributes in orig_termios.
 * A little sketchy because if enable_raw_mdde gets called twice,
 * This will overwrite the original with the raw mode attributes.
 */

void enable_raw_mode(void) {
    if(in_raw_mode) return;

    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        //printf("Enabling raw mode\r\n");
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    struct termios raw = orig_termios;


    // Maybe set termios flags manually in future, but cfmakeraw works right now
    cfmakeraw(&raw);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
    in_raw_mode = true;
}

/*
 * Restores orig_termios saved in enable_raw_mode
 */
void disable_raw_mode(void) {
    //printf("disabling raw mode\r\n");
    if(isatty(STDIN_FILENO) && tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        perror("tcsetattr");
    }
    in_raw_mode = false;
}

