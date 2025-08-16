#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "./tokenize.h"
#include "../list/list.h"

/*** TOKENIZER ***/

static const char *DELIMS = " \t\r\n";
static const char *OPERATORS = "|><&";

void add_token(struct token_list *list, struct token t) {
    if(list->size >= list->capacity) {
        printf("size: %d, capac: %d\r\n", list->size, list->capacity);
        printf("reallocating....\r\n");
        list->tokens = realloc(list->tokens, sizeof(struct token) * list->capacity*2);
        list->capacity *= 2;
    }
    if(list->size < list->capacity) {
        list->tokens[list->size] = t; 
        list->size++;
    }
}

void free_token_list(struct token_list *list) {
    for(int i = 0; i < list->size; i++ ) {
        free(list->tokens[i].value);
    }
    free(list->tokens);
    free(list);
}


/*
 * Expected behavior.
 * ls -ali.             ls and -ali are two tokens
 * ls       -ali.       ls and -ali are two tokens
 * "ls -ali"            "ls -ali" is one token
 * ls -a"li"            ls is one token and -ali is one token
 */

void init_token_list(struct token_list *tl)
{
    long init_size = 4;

    tl->tokens = malloc(sizeof(struct token) * init_size);
    tl->capacity = init_size;
    tl->size = 0;
}

void add_token_from_buff(struct token_list *tl, char *buf, size_t bufsize, enum type type)
{
    if(strlen(buf) == 0) {
        return;
    }
    struct token t;
    fflush(stdout);
    t.value = strdup(buf);
    t.type = type;
    memset(buf, 0, bufsize);
    add_token(tl, t);

}

/*
 * Given a linked list of text, deliminated by whitespace,
 * produce an array of tokens
 * These tokens have a string and a token type of word, operator, or string.
 * Maybe more types will be added later.
 * TODO Would prefer this to be flattened a bit
 */

int get_token_operator_type(const char* buf) {
    if(strcmp(buf, "|") == 0) {
        return TOK_PIPE;
    }
    if(strcmp(buf, ">") == 0) {
        return TOK_REDIR_RIGHT;
    }
    if(strcmp(buf, "<") == 0) {
        return TOK_REDIR_LEFT;
    }
    return TOK_OPERATOR;
}
struct token_list *tokenize(struct list *line) {//, size_t size) {
    enum state state = NORMAL;
    long init_size = 4;

    struct token_list *tl = malloc(sizeof(struct token_list));
    init_token_list(tl);
    char buf[32];
    buf[0] = '\0';
    int i = 0;
    struct node *cursor = line->HEAD;

    if(cursor->c == '\0' && cursor->next != NULL) {
        cursor = cursor->next;
    }

    while(cursor != NULL ) {
        char c = cursor->c;
        if (state == NORMAL) {
            if(strchr(DELIMS, c)) {
                add_token_from_buff(tl, buf, sizeof(buf), TOK_WORD);
                i = 0;

                // skip over rest of delims
                while(cursor != NULL && strchr(DELIMS, c)) {
                    cursor = cursor->next;
                    if(cursor != NULL) {
                        c = cursor->c;
                    }
                }
            } else if (strchr(OPERATORS, c)) {
                // Consume current buffer to token list
                // to clear buffer for token
                add_token_from_buff(tl, buf, sizeof(buf), TOK_WORD);
                i = 0;
                buf[i++] = c;

                // Handle operators && ||.
                if (cursor->next != NULL && cursor->next->c == c) {
                    buf[i++] = c; 
                    cursor = cursor-> next;
                }
                if (cursor->next != NULL) {
                    cursor = cursor-> next;
                }

                buf[i] = '\0';
                add_token_from_buff(tl, buf, sizeof(buf), get_token_operator_type(buf));
                i = 0;
            } else if (c == '"') {
                state = IN_QUOTE;
                cursor = cursor->next;
            } else {
                buf[i++] = c;
                buf[i] = '\0';
                cursor = cursor->next;
            }
        } else if (state == IN_QUOTE) {
            if (c == '"') {
                add_token_from_buff(tl, buf, sizeof(buf), TOK_STRING);
                state = NORMAL;

                if (cursor->next != NULL) {
                    cursor = cursor->next;
                    c = cursor->c;
                }

                while(cursor != NULL && strchr(DELIMS, cursor->c)) {
                    cursor = cursor->next;
                }

                i = 0;
            } else {
                buf[i++] = c;
                buf[i] = '\0';
                cursor = cursor->next;
            }
        }
    }

    // Debug print
    for (int i = 0; i < tl->size; i++) {
        printf("Token of type %d %d: %s\r\n", tl->tokens[i].type, i, tl->tokens[i].value);
    }
    free_list(line);
    return tl;
}
