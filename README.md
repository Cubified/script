## script

A generic, configuration-free script dispatcher, useful for running init scripts in parallel.

I wrote this because I wanted a custom init which both booted quickly and was pleasing to look at (two goals which generally tend to be at odds with one another...).  While the core of this project is not an init, the scripts provided in `init.d/` (and `halt.d/`), coupled with [fpinit](https://github.com/Cubified/fpinit) (my modified version of [sinit](https://core.suckless.org/sinit)), can boot (and halt) a usable system.

### Demo

![demo.gif](https://github.com/Cubified/script/blob/master/demo.gif)

### Compiling and Running

`script` has no dependencies, meaning it can be compiled and run with:

     $ make
     $ ./script [script directory]

`config.h` allows for modification of the header text, with the default ANSI 8-color palette available as macros (visible in `script.c`).

### Runlevels/Parallel Execution

`script` groups scripts (which it calls "jobs") by runlevel, determined by the first character of each script's filename.

If the first character of a given filename is between 0 and 9, that job is grouped into the appropriate runlevel, with 0 executing first and 9 executing last.  All other jobs run in parallel with one another after this.

In effect, `script` is usable without renaming or modifying any files, while also supporting sequential runlevels.

### Title file

In the directory of scripts to be executed, a file with the name `"title"` can be created which allows for jobs to have more user-friendly names or descriptions.  Its file format is as follows:

     script-filename.sh: Name or description
     example-two.sh: Hello world

### To-Do

- Support overflow -- if more scripts execute than there are columns in the output terminal, visual artifacts appear
- `stat()` files to check for executable bit
