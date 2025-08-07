#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../term/term.h"
#include "../parser/parser.h"
#include "./builtins/builtins.h"
#include "exec.h"


//TODO cleanup and split logic
void exec_tree(struct ot_node *node)
{
    int pipefd[2];
    pid_t cpid1, cpid2;
    
    if(node->type == PIPE) {
        if(pipe(pipefd) == -1) {
            // pipe error
        }

        cpid1 = fork();
        if(cpid1 == -1) {
            // fork error
        }

        if(cpid1 == 0) {
            // in first child (write end fd=1)
            // exec smth smth 
            struct ot_node *left = node->children.left_child;
            struct ot_node *right = node->children.right_child;

            if(left == NULL || right == NULL) {
                // pipe invalid args
            }
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            exec_tree(left);
            exit(1);
        } 
        // in parent
        cpid2 = fork();

        if(cpid2 == 0) {
            // in second child (read end fd=2)
            struct ot_node *left = node->children.left_child;
            struct ot_node *right = node->children.right_child;

            if(left == NULL || right == NULL) {
                // pipe invalid args
            }
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            exec_tree(right);
            exit(1);
        }

        else {
            // in parent
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(cpid1, NULL, 0);
            waitpid(cpid2, NULL, 0);
        }

        //fork && pipe

    } else if (node->type == COMMAND) {
        // exec cmd?
        if(strcmp(node->cmd->argv[0], "cd") == 0) {
            cd(node->cmd->argv[1]);
            //printf("built-ins\r\n");
        } else if(strcmp(node->cmd->argv[0], "exit") == 0) {
            // 
            //printf("exiting\r\n");
            disable_raw_mode();
            exit(0);
            //exec_exit(tl);
        }
        pid_t cpid;
        cpid = fork();
        if(cpid == 0) {
            //disable_raw_mode();
            if(execvp(node->cmd->argv[0], node->cmd->argv) == -1) {
                //printf("Error executing %s\r\n", node->cmd->argv[0]);
                exit(1);
            }
        }


        waitpid(cpid, NULL, 0);
    }
}


void exec_exit(struct token_list *tl)
{
    printf("In exit function\r\n");
    free_tokens(tl); 
    exit(0);
}
