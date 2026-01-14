# Compiler and Flags
CC = gcc
CFLAGS = -fPIC -Wall -Wextra
LDFLAGS = -shared

# Directories (Absolute paths using PWD) 
INC_DIR = $(shell pwd)/include
LIB_DIR = $(shell pwd)/lib

# Environment variable for the compiler
export C_INCLUDE_PATH := $(INC_DIR):$(C_INCLUDE_PATH)

# Default target
all: prepare $(LIB_TARGET) main

main: main.c
	$(CC) main.c -o trojanbuilder

prepare:
	mkdir -p $(LIB_DIR) 
	mkdir -p $(BIN_DIR)

libs: stub.c
	$(CC) stub.c $(CFLAGS) $(LDFLAGS) -o $(LIB_DIR)/libstub.so

demo:  foreground.c  background.c
	$(CC) background.c $(CFLAGS) -o warrior
	$(CC) foreground.c $(CFLAGS) -o horse

clean:
	rmdir $(LIB_DIR) 2>/dev/null || true

.PHONY: all prepare clean
