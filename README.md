# Dependencies
All you need is a decent C compiler (gcc, etc.) and the Raylib or SDL library.
# Building and running
For both versions, you need to compile with -lm.

For the Raylib version, you need to compile with -lraylib.

`$ gcc -o func-bounce func-bounce.c -lraylib -lm`

For the SDL version, you need to compile with -SDL2.

`$ gcc -o func-bounce func-bounce.c -lSDL2 -lm`

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
