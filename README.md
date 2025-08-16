I'm writing this shell in C for learning purposes.

Current features
* Line editing with left/right arrows and backspace
* Quotes for multi-word tokens
* Pipe chaining implemented via recursive descent
* Input output redirection via > < operators

Compatability
* Not good.
* Uses hardcoded ANSI escape sequences for the line-reading functionality because I wanted to learn how it works. Using ncurses or just readline would have allowed for it to be more portable 

Todo
* At some point need to stop expecting good input
    * Ex: Unbalanced quotes, pipes
* Add other operators such as && || ;
* Job management.
* Signal handling. (Ctrl L to clear screen, Ctrl C to newline)
