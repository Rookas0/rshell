#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "./tokenize.h"
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

struct token_list tokenize(struct list *line) {//, size_t size) {
    // TODO: implement quotation marks.
    long init_size = 4;
    struct token_list tl = {
        .tokens = NULL,
        .capacity = init_size,
        .size = 0
    };
    tl.tokens = malloc(sizeof(struct token) * init_size);
    char buf[32];
    buf[0] = '\0';
    // read line. when space hit 
    //printf("Line is %ld long\r\n", strlen(line));
    int j = 0;
    struct list *ol = line;
    if(line->c == '\0' && line->next != NULL) {
        line = line->next;
    }

    do {
        char c = line->c;
        if(c != ' ') {
            buf[j] = c;
            buf[j+1] = '\0';
        }
        if(c == ' ' || c == '\r' || c == '\0' || line->next == NULL) {
            struct token t;
            fflush(stdout);
            t.value = strdup(buf);
            memset(buf, 0, sizeof(buf));
            j = 0;
            add_token(&tl, t);
            line = line->next;
            continue;
        }
        fflush(stdout);
        line = line->next;
        j++;
        if(c == '\r') break;
    }while (line != NULL && line->c != '\0');
    for (int i = 0; i < tl.size; i++) {
        printf("Token %d: %s\r\n", i, tl.tokens[i].value);
    }
    free_list(ol);
    return tl;
}
