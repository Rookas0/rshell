#pragma once
#include "../list/list.h"

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

struct string {
    int capac;
    int size;
    char *s;
};
struct line {
    struct string str;
    int posx;
};

struct list * readline(char *prompt);
