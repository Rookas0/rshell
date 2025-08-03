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
void free_tokens(struct token_list *list);
struct token_list *tokenize(struct list *line);
