#pragma once
#include <stdbool.h>
#include <stdlib.h>

struct list {
    char c;
    bool is_cp;
    struct list *next;
    struct list *prev;
};

struct list *create_list(char c);
void free_list(struct list* l);
