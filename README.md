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

```

To generate a `compile_commands.json` file for better integration with code editors and tools, you can run CMake with the following command:
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMAND
S=ON ..
```
