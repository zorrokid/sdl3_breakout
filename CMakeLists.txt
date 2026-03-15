cmake_minimum_required(VERSION 3.16)
project(SDL3_Breakout C)

# Use C11 standard for better modern C support
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 1. Find the SDL3 packages
# This replaces $(shell pkg-config --cflags --libs sdl3)
find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

# 2. Define the executable and its source files
add_executable(breakout 
    main.c 
    bricks.c 
    ball.c 
    paddle.c 
    particles.c 
    events.c
)

# 3. Link the libraries
# This automatically handles the -I (includes) and -l (linking) flags
target_link_libraries(breakout 
    PRIVATE 
        SDL3::SDL3 
        SDL3_ttf::SDL3_ttf
)

# 4. Copy assets to build directory (Optional but helpful)
# This makes sure your .ttf files are where the executable can find them
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
