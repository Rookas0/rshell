#pragma once
#include "../list/list.h"
struct token {
    char *value;
};

struct token_list {
    struct token *tokens;
    int size;
    int capacity;
};
void add_token(struct token_list *list, struct token t);
void add_token_from_buff(struct token_list *tl, char *buf, size_t bufsize);
void free_tokens(struct token_list *list);
struct token_list *tokenize(struct list *line);
