#pragma once

#include "../util/util.h"

enum shell_key {
    ARROW_LEFT =  1000,
    ARROW_DOWN,
    ARROW_UP,
    ARROW_RIGHT,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

struct line {
    struct string str;
    int posx;
};

struct line * readline(char *prompt);
void free_line(struct line *ln);
