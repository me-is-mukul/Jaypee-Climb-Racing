# Compiler and flags
CC = g++
CFLAGS = -O2 -Wall

# Raylib paths (change if installed somewhere else)
RAYLIB_DIR = C:/raylib/raylib/src
INCLUDES = -I$(RAYLIB_DIR)
LIBS = -L$(RAYLIB_DIR) -lraylib -lopengl32 -lgdi32 -lwinmm

# Source and output
SRC = main.cpp
OUT = main.exe

# Default target
all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(OUT) $(LIBS)

# Clean target
clean:
	del $(OUT)
