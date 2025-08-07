I'm writing this shell in C for learning purposes.

Current features
* Line editing with left/right arrows and backspace
* Quotes for multi-word tokens
* Pipe chaining implemented via recursive descent

Todo
* At some point need to stop expecting good input
    * Ex: Unbalanced quotes, pipes
* Add input output redirection
* Add other operators such as && || ;
* Job management.
* Signal handling. (Ctrl L to clear screen, Ctrl C to newline)
