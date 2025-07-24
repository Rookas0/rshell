#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*** TOKENIZER ***/

struct token {
    char *value;
};
struct token_list {
    char **tokens;
    int size;
    int capacity;
};

void add_token(struct token_list *list, char *t) {
    if(list->size >= list->capacity) {
        printf("size: %d, capac: %d\n", list->size, list->capacity);
        printf("reallocating....\n");
        list->tokens = realloc(list->tokens, sizeof(struct token) * list->capacity*2);
        list->capacity *= 2;
    }
    if(list->size < list->capacity) {
        list->tokens[list->size] = t; 
        list->size++;
    }
}

struct token_list tokenize(char *line, size_t size) {
    // let's start with enough space for 5 tokens, and if there's more allocate
    long init_size = 4;
    struct token_list tl = {
        .tokens = NULL,
        .capacity = init_size,
        .size = 0
    };
    tl.tokens = malloc(sizeof(struct token) * init_size);
    char buf[32];
    // read line. when space hit 
    for(int i = 0, j = 0; line [i] != '\0'; i++) {
        // if normal char (not whitespace or newline or eof, add to token
        //printf("i: %d, char: %c\n", i, line[i]);
        if(line[i] == ' ' || line[i] == '\n') {
            char *t;
            t = strdup(buf);
            //memcpy(t.value, buf, strlen(buf));
            memset(buf, 0, sizeof(buf));
            j = 0;
            add_token(&tl, t);
            continue;
        }
        buf[j] = line[i];
        buf[j+1] = '\0';
        j++;
        if(line[i] == '\n') break;
    }
    add_token(&tl, NULL);
    for (int i = 0; i < tl.size; i++) {
        printf("Token %d: %s\n", i, tl.tokens[i]);
    }

    return tl;
}


int handle_tokens(struct token_list tl) {
    /*
    char *args[2];
    char buf[20];

    args[0] = buf;
    //printf("%s\n", buf);
    args[1] = NULL;
    if(size > 0) {
        args[1] = NULL;
    }
    */
    //printf("%s\n", args[0]);    
    fflush(stdout);
    // need to fork
    execvp(tl.tokens[0], tl.tokens);
    return 0;
}
int main(int argc, char *argv[])
{
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;
    struct token_list tl;
    for(;;) {
        printf("> ");
        getline(&line, &size, stdin);
        tl = tokenize(line, size);
        handle_tokens(tl);
        //handle line
        // read input
    }
    return EXIT_SUCCESS;
}
