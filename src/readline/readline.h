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

struct line_info {
    int posx;
};

extern struct line_info line_info;
void print_prompt(struct list *head, char * prompt);
int readchar(void);
void insert_char_at_cursor(struct list *lst, char c);
void move_cursor_left(struct list *lst);
void move_cursor_right(struct list *lst);
void delete_at_cursor(struct list *lst);
void handle_char(struct list *lst, int nc);
struct list * readline(char *prompt);
