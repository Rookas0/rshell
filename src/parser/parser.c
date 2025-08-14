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
        cmd->capacity = cmd->capacity ? cmd->capacity * 2 : 4;
        cmd->argv = realloc(cmd->argv, sizeof(char*) * cmd->capacity);
    }
    if(cmd->argc < cmd->capacity) {
        cmd->argv[cmd->argc] = arg; 
        cmd->argc++;
    }
}

struct token* peek(struct token_stream *ts)
{
    if(ts->pos < ts->list->size) {
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

/* ====================== Recursive Descent Parser ====================== */
/* 
 * Start at item with lowest precedence
 * Lowest precedence items call items of higher precedence
 * This ensures more tightly bound operators are handled first
 * in the future when more are added.
 * Ex: ls -ali | grep foo | wc -l
 *     parser reads first token, ls
 *     starts in parse_pipe, and calls parse_cmd
 *     parse_cmd consumes the command, then returns
 *     parse_pipe loops, calling parse_cmd until all pipes are handled
 *
 * ====================================================================== */

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

    if(tk->type != TOK_WORD && tk->type != TOK_STRING) return NULL;
    while(tk != NULL && (tk->type == TOK_WORD || tk->type == TOK_STRING)) {
        add_to_cmd(cmd, strdup(tk->value));
        advance(ts);
        tk = peek(ts);
    }
    add_to_cmd(cmd, NULL);

    struct ot_node* node = malloc(sizeof(struct ot_node));
    node->type = COMMAND;
    node->cmd = cmd;
    return node;
}

struct ot_node* parse(struct token_list *tl)
{   
    long total_size = 0;
    struct token_stream ts;

    ts.pos = 0;
    ts.list = tl;

    // call lowest precedence. in the future this call will be changed
    struct ot_node *node = parse_pipe(&ts);

    return node;
}
