#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "./util.h"

void init_string(struct string *str, int capac)
{
    str->capac = capac;
    str->size = 0;

    str->s = malloc(sizeof(char) * (str->capac + 1));
    if (!str->s) {
        perror("malloc");
    }
    str->s[0] = '\0';
}

bool insert_char_to_str(struct string *str, char c, int pos)
{

    if (str->size >= str->capac) {
        str->capac *= 2;
        char *tmp = realloc(str->s, sizeof(char) * (str->capac + 1));
        if (!tmp) {
            perror("realloc");
            return false;
        }
        str->s = tmp;
    }

    //shift
    for (int i = str->size; i >= pos; i--) {
        str->s[i+1] = str->s[i];
    }

    str->size++;
    str->s[pos] = c;
    str->s[str->size] = '\0';
    return true;
}
