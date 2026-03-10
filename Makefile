# Variables for compiler and flags
CC = gcc
CFLAGS = $(shell pkg-config --cflags sdl3) -Wall -Wextra
LIBS = $(shell pkg-config --libs sdl3)

# The name of your final program
TARGET = breakout

# All your .c files
SRCS = main.c bricks.c

# The default rule (what happens when you just type 'make')
all:
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LIBS)

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)
