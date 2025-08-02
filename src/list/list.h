#pragma once
#include <stdbool.h>
#include <stdlib.h>

struct list {
    struct node *HEAD;
    struct node *cursor;
};

struct node {
    char c;
    bool is_cp;
    struct node *next;
    struct node *prev;
};

struct list *create_list(void);
struct node *create_node(char c);

bool list_move_cursor_left(struct list *lst);
bool list_move_cursor_right(struct list *lst);
bool list_delete_at_cursor(struct list *lst);
void free_list(struct list* l);
