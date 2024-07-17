# Dependencies
All you need is a decent C compiler (gcc, etc.) and the Raylib library.
# Building and running
You need to compile with -lraylib and -lm.

`$ gcc -o func-bounce func-bounce.c -lraylib -lm`

From there, you can run it from the terminal with `./func-bounce` or just clicking on the program in your file manager.
# Changing the graph
To change the graph of the function, simply change f(x).

**NOTE: You must account for the screen dimensions when changing f(x).**

```
float f(float x) {
    return x*x/80 - 80;
}
```
Afterwards, recompile the program.
