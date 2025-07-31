/*** BUILT-INS ***/

#include <stdio.h>
#include <unistd.h>

#include "builtins.h"

int cd(char * path) {
    if(chdir(path) == -1) {
        perror("chdir");
        return 1;
    }
    return 0;
}
