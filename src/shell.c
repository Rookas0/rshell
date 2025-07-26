#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#include <stdbool.h>

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
/*** line reader ***/
struct list {
    char c;
    bool is_cp;
    struct list *next;
    struct list *prev;
};

struct list *create_list(char c) {
    struct list *my_list = malloc(sizeof(struct list));

    my_list->next = NULL;
    my_list->prev = NULL;
    my_list->is_cp = true;
    my_list->c = c;

    return my_list;
}

void free_list(struct list* l) {
    while(l->prev != NULL) {
        l = l->prev;
    }
    struct list *next;
    while(l != NULL) {
        next = l->next;
        free(l);
        l = next;
    }
}

/*** termios ***/ 
struct termios orig_termios;
void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode(void) {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    struct termios raw = orig_termios;

    atexit(disable_raw_mode);

    cfmakeraw(&raw);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

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
    // let's start with enough space for 5 tokens, and if there's more allocate
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
    /*
    for(int i = 0, j = 0; i <= strlen(line); i++) {
        // if normal char (not whitespace or newline or eof, add to token
        //printf("i: %d, char: %c\r\n", i, line[i]);
        //fflush(stdout);
        if(line[i] == ' ' || line[i] == '\r' || line[i] == '\0') {
            struct token t;
            printf("Adding %s\r\n", buf);
            fflush(stdout);
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
    */
    //add_token(&tl, (struct token) {.value = NULL});
    for (int i = 0; i < tl.size; i++) {
        printf("Token %d: %s\r\n", i, tl.tokens[i].value);
    }
    free_list(ol);
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
    printf("In exec_cmd\r\n");
    fflush(stdout);
    char **argv = malloc(sizeof(char *) * (tl->size + 1));
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
        disable_raw_mode();
        if(execvp(argv[0], argv) == -1) {
            printf("Error executing %s\r\n", argv[0]);
            exit(1);
        }
    } else {
        wait(NULL);
        free(argv);
        enable_raw_mode();
    }

    return 0;
}

void exec_exit(struct token_list *tl) {
    printf("In exit function\r\n");
    free_tokens(tl); 
    exit(0);
}

int handle_tokens(struct token_list tl) {
    char * cmd = NULL;
    cmd = tl.tokens[0].value;
    if(strcmp(cmd, "cd") == 0) {
        cd(tl.tokens[1].value);
        printf("built-ins\r\n");
    } else if(strcmp(cmd, "exit") == 0) {
        exec_exit(&tl);
        printf("Why are we here...\r\n");
    }
    else {
        printf("Exec else\r\n");
        exec_cmd(&tl);
    }
    return 0;
}

void print_prompt(void) {
    printf("> ");
}

void intHandler(int sig) { 
    //write(STDOUT_FILENO, "\r\n> ", 3);
    write(STDOUT_FILENO, "\r\n> ", 4);
}

void seg_handler(int sig) {
    disable_raw_mode();
    printf("\nseg fault\n");
    exit(1);
}
    

void append_list(struct list* ll) {
    
}
// read key press as int.
// process key press. 
//
int readchar(void) {
    // referenced kilo text editor for handling escape sequence
    char c;
    read(STDIN_FILENO, &c, 1);

    // if escape sequence isn't followed by anything, escape key was pressed, so return escape
    if(c == '\x1b') {
        char seq[3];
        if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
        if(seq[0] == '[') {
            if(seq[1] >= '0' && seq[1] <= '9') {
                if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if(seq[2] == '~') {
                    switch(seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            } else {
                switch(seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        } else if(seq[0] == 0) {
            switch(seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
    return '\x1b';
    }
    return c;
}
void handle_char(struct list **llp, int nc) {
    struct list *ll = *llp;
    if(nc >= 0x20 && nc <= 0x7E) {
        char c = (char) nc;
        //printable
        if(ll->c != '\0') {
            struct list *nl = create_list(c);
            nl->prev = ll;
            ll->next = nl;
            *llp = nl;
        }
        else {
            ll->c = c;
        }
        write(STDOUT_FILENO, &((*llp)->c), 1);
    }
}
struct list * readline(void) {
    char c = '\0';
    struct list *ll = create_list('\0');
    struct list *cursor = ll;
    for(;;) {
        fflush(stdout);
        //read(STDIN_FILENO, &c, 1);
        int nc = readchar();
        handle_char(&ll, nc);
        c = (char) nc;

        if(c == '\r' || c == '\n') {
            //buf[i] = '\0';
            break;
        }
    }
    write(STDOUT_FILENO, "\r\n", 2);
    
    /*
    while(cursor->next != NULL) {
        write(STDOUT_FILENO, &cursor->c, 1);
        cursor = cursor->next;
    } 
    */
    return cursor;
}


int main(void)//int argc, char *argv[])
{
    enable_raw_mode();
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;
    //ssize_t nread;
    struct token_list tl;
    signal(SIGSEGV, seg_handler);
    signal(SIGINT, intHandler);
    for(;;) {
        print_prompt();
        fflush(stdout);
        //nread = getline(&line, &size, stdin);
        /*
        if(nread == -1) {
            if(errno == EINTR) {
                printf("getline int\r\n");
                continue;
            }
            else {
                perror("getline");
                break;
            }
        }
        */
        struct list *line = readline();
        tl = tokenize(line); //, size);
        printf("After tokenize\r\n");
        handle_tokens(tl);
        free_tokens(&tl);
        //handle line
        // read input
    }
    return EXIT_SUCCESS;
}
