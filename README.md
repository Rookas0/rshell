# Rshell

## Current features
1. Line editing with left/right arrows and backspace
2. Quotes for multi-word tokens
3. Pipe chaining implemented via recursive descent
4. Input output redirection using > < operators
5. cd & exit built-ins

## Building

You'll need:
   * cmake
   * a C compiler. I used GCC.
### Build steps
1. Clone the repository:
```bash
git clone https://github.com/Rookas0/rshell
cd rshell
```
2. Create a build directory and configure
```bash
mkdir build && cd build
cmake ..
```
3. Compile
``bash
make
```
4. Run rshell!
```bash
./rshell
```

## Compatability
* Not good.
* Uses hardcoded ANSI escape sequences for the line-reading functionality because I wanted to learn how it works. Using ncurses or just readline would have allowed for it to be more portable 

## TODO
* At some point need to stop expecting good input
    * Ex: Unbalanced quotes, pipes
* Add other operators such as && || ;
* Job management.
* Signal handling. (Ctrl L to clear screen, Ctrl C to newline)
* Allow line to be longer than terminal width (wrapping)
* Allow tokens of arbitrary length (capped buffer right now)
* More rich line editing
    * Tab completion
    * Shortcuts to start and end of line
