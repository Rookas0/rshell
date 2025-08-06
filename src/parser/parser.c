#include "../tokenize/tokenize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"


struct command* init_cmd(int capac)
{
    struct command* cmd = malloc(sizeof(struct command));
    cmd->argv = NULL;
    cmd->argc = 0;
    cmd->capacity = 0;
    return cmd;
}

void free_operator_tree(struct ot_node *node)
{
    // if node child, call func on node child.
    pid_t cpid;
    if(node->type == PIPE) {

        struct ot_node *left = node->children.left_child;
        struct ot_node *right = node->children.right_child;

        free_operator_tree(node->children.left_child);
        free_operator_tree(node->children.right_child);
    } else if (node->type == COMMAND) {
        struct command *cmd = node->cmd;
        for(int i = 0; i < cmd->argc; i++) {
            free(cmd->argv[i]);
        }
        free(cmd->argv);
        free(node->cmd);
    }
    free(node);
}

void add_to_cmd(struct command *cmd, char *arg)
{
    if(cmd->argc >= cmd->capacity) {
        //printf("argc: %d, capac: %d\r\n", cmd->argc, cmd->capacity);
        //printf("reallocating....\r\n");
        cmd->capacity = cmd->capacity ? cmd->capacity * 2 : 4;
        cmd->argv = realloc(cmd->argv, sizeof(char*) * cmd->capacity);
    }
    if(cmd->argc < cmd->capacity) {
        cmd->argv[cmd->argc] = arg; 
        cmd->argc++;
    }
}

/*
 * for int i = 0 i < tl->size; i++ 
 *     if token == pipe
 *         read up until token and concat into cmd argv
 *         read after pipe until EOL or another pipe reached. store this into cmd argv
 */
struct token* peek(struct token_stream *ts)
{
    if(ts->pos < ts->list->size) {
        //printf("returning in peek\r\n"); //debug
        return &ts->list->tokens[ts->pos];
    }

    return NULL;
}

void advance(struct token_stream *ts)
{
    if(ts->pos < ts->list->size) {
        ts->pos++; 
    }
}

struct ot_node* parse_pipe(struct token_stream *ts)
{
    struct ot_node* left = parse_cmd(ts);
    struct token *tk = peek(ts);

    while(tk != NULL && tk->type == TOK_PIPE) {
        advance(ts);
        struct ot_node* right = parse_cmd(ts);

        tk = peek(ts);

        struct ot_node* pipe_node = malloc(sizeof(struct ot_node));
        pipe_node->type = PIPE;
        pipe_node->children.left_child = left;
        pipe_node->children.right_child = right;
        left = pipe_node;
    }
    return left;
}

struct ot_node* parse_cmd(struct token_stream *ts)
{
    struct token *tk = peek(ts);
    struct command *cmd = init_cmd(4);

    if(tk->type != TOK_WORD) return NULL;

    while(tk != NULL && (tk->type == TOK_WORD || tk->type == TOK_STRING)) {
        printf("adding to cmd\r\n");
        add_to_cmd(cmd, strdup(tk->value));
        advance(ts);
        tk = peek(ts);
    }
    add_to_cmd(cmd, NULL);

    // debug print cmd contents
    /*
    printf("debug print\r\n");
    for (int i = 0; i < cmd->argc; i++) {
        printf("%s", cmd->argv[i]);
        printf("\r\n");
    }
    *///

    struct ot_node* node = malloc(sizeof(struct ot_node));
    node->type = COMMAND;
    node->cmd = cmd;

    return node;
}

struct ot_node* parse(struct token_list *tl)
{   
    // read until end of list or operator
    //  when reachd, add all read tokens into a command struct
    long total_size = 0;
    struct token_stream ts;
    ts.pos = 0;
    ts.list = tl;
    struct ot_node *node = parse_pipe(&ts);
    free_tokens(tl);

    return node;
}
