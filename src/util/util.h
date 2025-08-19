#pragma once

#include <stdbool.h>
struct string {
    int capac;
    int size;
    char *s;
};

void init_string(struct string *str, int capac);
bool insert_char_to_str(struct string *str, char c, int pos);
