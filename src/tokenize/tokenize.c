#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenize.h"
#include "util.h"

/*** TOKENIZER ***/

#define STR_INIT_CAPAC 32
static const char *DELIMS = " \t\r\n";
static const char *OPERATORS = "|><&";

void add_token(struct token_list *list, struct token t)
{
    if(list->size >= list->capacity) {
        //printf("size: %d, capac: %d\r\n", list->size, list->capacity);
        //printf("reallocating....\r\n");
        list->tokens = realloc(list->tokens, sizeof(struct token) * list->capacity*2);
        list->capacity *= 2;
    }
    if(list->size < list->capacity) {
        list->tokens[list->size] = t; 
        list->size++;
    }
}

void free_token_list(struct token_list *list)
{
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

int get_token_operator_type(const char* buf)
{
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

struct token_list *tokenize(char *line)
{
    enum state state = NORMAL;
    long init_size = 4;

    struct token_list *tl = malloc(sizeof(struct token_list));

    struct string token_str;
    init_string(&token_str, STR_INIT_CAPAC);
    init_token_list(tl);
    char buf[64];
    buf[0] = '\0';
    int token_pos = 0;
    int line_len = strlen(line);

    int i = 0;

    //printf("%d\r\n", line_len);
    while (i < line_len) {
        //printf("%d\r\n", i);
        char c = line[i];
        if (state == NORMAL) {
            if(strchr(DELIMS, c)) {
                //printf("in delims normal\r\n");
                add_token_from_buff(tl, token_str.s, token_str.size, TOK_WORD);
                token_pos = 0;
                token_str.size = 0;

                // skip over rest of delims
                  while(i < line_len && strchr(DELIMS, c)) {
                      i++;
                      c = line[i];
                  }
            } else if (i == line_len - 1) {
                //printf("in i == line_len\r\n");
                insert_char_to_str(&token_str, c, token_pos++);
                add_token_from_buff(tl, token_str.s, token_str.size, TOK_WORD);
                token_pos = 0;
                token_str.size = 0;
                i++;
                break;
            }else if (strchr(OPERATORS, c)) {
                add_token_from_buff(tl, token_str.s, token_str.size, TOK_WORD);
                token_pos = 0;
                token_str.size = 0;
                insert_char_to_str(&token_str, c, token_pos++);
                i++;
                if(i < line_len && line[i] == c) {
                    insert_char_to_str(&token_str, c, token_pos++);
                    i++;
                }

                add_token_from_buff(tl, token_str.s, token_str.size, get_token_operator_type(token_str.s));
            } else if (c == '"') {
                state = IN_QUOTE;
                i++;
            } else {
                insert_char_to_str(&token_str, c, token_pos++);
                i++;
            }
        } else if (state == IN_QUOTE) {
            //printf("In quote %d\r\n", i);
            if (c == '"') {
                //printf("Leaving quote %d\r\n", i);
                add_token_from_buff(tl, token_str.s, token_str.size, TOK_STRING);
                state = NORMAL;

                if (i <= line_len) {
                    i++;
                }
                c = line[i];
                while(i < line_len && strchr(DELIMS, c)) {
                    i++;
                    c = line[i];
                }

            } else {
                insert_char_to_str(&token_str, c, token_pos++);
                i++;
            }
        }
    }

    /*
    for(int i = 0; i < tl->size; i++) {
        printf("Token type %d %d: %s\r\n", i, tl->tokens[i].type, tl->tokens[i].value);
    }
    */
    return tl;
}
