#pragma once
#include "../list/list.h"

enum state { NORMAL, IN_QUOTE };

enum type {
    TOK_STRING,
    TOK_WORD,
    TOK_OPERATOR,
    TOK_PIPE,
    TOK_REDIR_RIGHT,
    TOK_REDIR_LEFT,
};

struct token {
    char *value;
    enum type type;
};

struct token_list {
    struct token *tokens;
    int size;
    int capacity;
};

//void add_token(struct token_list *list, struct token t);
//void add_token_from_buff(struct token_list *tl, char *buf, size_t bufsize, enum type type);
void free_token_list(struct token_list *list);
struct token_list *tokenize(struct list *line);
