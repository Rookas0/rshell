#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "./tokenize.h"
enum State { NORMAL, IN_QUOTE };

enum State state = NORMAL;
/*** TOKENIZER ***/

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

void free_tokens(struct token_list *list) {
    for(int i = 0; i < list->size; i++ ) {
        free(list->tokens[i].value);
    }
    free(list->tokens);
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

struct token_list *tokenize(struct list *line) {//, size_t size) {
    // TODO: implement quotation marks.
    long init_size = 4;
    //struct token_list *tl = init_token_list;

    printf("Hello\r\n");
    struct token_list *tl = malloc(sizeof(struct token_list));
    init_token_list(tl);
    char buf[32];
    buf[0] = '\0';
    int j = 0;
    struct list *ol = line;
    struct node *cursor = line->HEAD;

    // if at dummy node (should always be the case)
    if(cursor->c == '\0' && cursor->next != NULL) {
        cursor = cursor->next;
    }
    do {
        // if " hit, go into string state, and create a single token until " is reached
        char c = cursor->c;
        if(state == NORMAL) {
            if(c != ' ') {
                buf[j] = c;
                buf[j+1] = '\0';
                j++;
            }
            // delim
            if(c == ' ' || c == '\r' || c == '\0' || cursor->next == NULL) {
                printf("%d\r\n", strlen(buf));
                if(strlen(buf) == 0) {
                    continue;
                }
                struct token t;
                fflush(stdout);
                t.value = strdup(buf);
                memset(buf, 0, sizeof(buf));
                j = 0;
                add_token(tl, t);
                cursor = cursor->next;

                // enter delim state
                while(cursor != NULL && cursor->c == ' ' && cursor->next != NULL) {
                    cursor = cursor->next;
                    c = cursor->c;
                }
                continue;
            }
            if(c == '"') {
                state = IN_QUOTE;
                j = 0;
            }
            fflush(stdout);
            cursor = cursor->next;
            if(c == '\r') break;
        } else {
            printf("%c\r\n", c);
            fflush(stdout);
            if(c == '"') {
                state = NORMAL;
                struct token t;
                fflush(stdout);
                t.value = strdup(buf);
                memset(buf, 0, sizeof(buf));
                j = 0;
                add_token(tl, t);
                cursor = cursor->next;
                j++;
                continue;
            } else {
                buf[j] = c;
                buf[j+1] = '\0';
                j++;
                cursor = cursor->next;
            }
        }
    } while (cursor != NULL && cursor->c != '\0');
    for (int i = 0; i < tl->size; i++) {
        printf("Token %d: %s\r\n", i, tl->tokens[i].value);
    }
    free_list(ol);
    return tl;
}
