# What and Why?

Simple breakout clone written in C using SDL3. The game features basic mechanics of a breakout game, including a paddle, ball, and bricks. The goal is to break all the bricks without letting the ball fall below the paddle.

This project is for me to refresh my C skills with SDL3 and game development experiment. This is also some kind of trip way back to my roots before I started studying to be a software engineer. I used to study basics of game development and programming in general in C and SDL way back in early 2000s. While working in the industry it seems that new languages and frameworks are coming out every day (or rather they are AI tools these days) - from that point of view it's so refreshing to see that some things remain grounded and relevant throughout the years - C hasn't gone anywher and SDL is still going on strong! 

# Requirements

- GCC or Clang (supporting C11)
- CMake
- SDL3
- SDL_ttf 3

# Building

Configure the project using CMake:

```bash
mkdir build
cd build
cmake ..

Compile the project:
```bash
cmake --build .
```

# Development

```

To generate a `compile_commands.json` file for better integration with code editors and tools, you can run CMake with the following command:
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMAND
S=ON ..
```
