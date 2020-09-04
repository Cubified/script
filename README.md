## script

A generic, configuration-free script dispatcher, useful for running init scripts in parallel.

I wrote this to use in conjunction with my custom init [fpinit](https://github.com/Cubified/fpinit), aiming to create a PID 1 which both booted quickly and was pleasing to look at (two goals which generally tend to be at odds with one another...).

### Demo

![demo.gif](https://github.com/Cubified/script/blob/master/demo.gif)

### Compiling and Running

`script` has no dependencies, meaning it can be compiled and run with:

     $ make
     $ ./script [script directory]

`make debug` can be run in place of `make`, which will disable the redirection of command output to `/dev/null` -- this may cause visual artifacts, but also provides useful information when investigating failed scripts.

`config.h` allows for modification of nearly everything which is printed to the screen -- the ANSI color palette is available as defined in `script.c`.  Additionally, there are a number of preset configs available in the `configs/` directory which can be copied to `config.h` .

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
- Add pthreads support in place of `while` loops
