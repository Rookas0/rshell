#include <stdio.h>
#include <stdlib.h>

#include "./string.h"

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
