#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

/*** TOKENIZER ***/

struct token {
    char *value;
};
struct token_list {
    struct token *tokens;
    int size;
    int capacity;
};

void add_token(struct token_list *list, struct token t) {
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

struct token_list tokenize(char *line) {//, size_t size) {
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
        printf("i: %d, char: %c\n", i, line[i]);
        if(line[i] == ' ' || line[i] == '\n') {
            struct token t;
            t.value = strdup(buf);
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
    //add_token(&tl, (struct token) {.value = NULL});
    for (int i = 0; i < tl.size; i++) {
        printf("Token %d: %s\n", i, tl.tokens[i].value);
    }

    return tl;
}



/*** BUILT-INS ***/

int cd(char * path) {
    if(chdir(path) == -1) {
        perror("chdir");
        return 1;
    }
    return 0;
}

int exec_cmd(struct token_list *tl) {
    printf("In exec_cmd\n");
    fflush(stdout);
    char **argv = malloc(sizeof(char *) * tl->size + 1);
    for(int i = 0; i < tl->size; i++) {
        argv[i] = tl->tokens[i].value;
        argv[i+1] = NULL;
    }
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(pid == 0) {
        if(execvp(argv[0], argv) == -1) {
            printf("Error executing %s\n", argv[0]);
        }
    } else {
        wait(NULL);
    }

    return 0;
}

void exec_exit(void) {
    printf("In exit function\n");
    exit(0);
}

int handle_tokens(struct token_list tl) {
    char * cmd = NULL;
    cmd = tl.tokens[0].value;
    if(strcmp(cmd, "cd") == 0) {
        cd(tl.tokens[1].value);
        printf("built-ins\n");
    } else if(strcmp(cmd, "exit") == 0) {
        exec_exit();
    }
    else {
        printf("Exec else\n");
        exec_cmd(&tl);
    }
    return 0;
}

void print_prompt(void) {
    printf("> ");
}

void intHandler(int sig) { 
    //write(STDOUT_FILENO, "\n> ", 3);
    write(STDOUT_FILENO, "\n> ", 3);
}


int main(void)//int argc, char *argv[])
{
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;
    //ssize_t nread;
    struct token_list tl;
    signal(SIGINT, intHandler);
    for(;;) {
        print_prompt();
        fflush(stdout);
        nread = getline(&line, &size, stdin);
        /*
        if(nread == -1) {
            if(errno == EINTR) {
                printf("getline int\n");
                continue;
            }
            else {
                perror("getline");
                break;
            }
        }
        */
        tl = tokenize(line); //, size);
        handle_tokens(tl);
        //handle line
        // read input
    }
    return EXIT_SUCCESS;
}
