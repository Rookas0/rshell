#pragma once

struct string {
    int capac;
    int size;
    char *s;
};

static void init_string(struct string *str, int capac);
