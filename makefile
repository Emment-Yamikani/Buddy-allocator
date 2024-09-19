# Compiler
CC := gcc
LD := ld
AR := ar
AS := as

# Common flags
CFLAGS := -O2 -g -std=gnu2x -Wall -Werror -Wextra
CPPFLAGS :=
APP_FLAGS := $(CFLAGS) $(CPPFLAGS)

# Directories
SRC_DIR := src
BIN_DIR := bin

# App source files
SOURCES := $(shell find $(SRC_DIR) -type f \( -name '*.c' -o -name '*.asm' -o -name '*.S' \))
OBJS := $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(patsubst $(SRC_DIR)/%.asm, $(SRC_DIR)/%.o, $(patsubst $(SRC_DIR)/%.S, $(SRC_DIR)/%.o, $(SOURCES))))

# App linked objects
LINKED_OBJS := $(OBJS)

# Make rules
all: app run

# App rules
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(APP_FLAGS) -MD -c $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.asm
	nasm $< -f elf64 -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.S
	$(CC) $(APP_FLAGS) -MD -c $< -o $@

app: $(BIN_DIR)/app

$(BIN_DIR)/app: $(LINKED_OBJS)
	$(CC) $^ -o $@ -pthread

run:
	./$(BIN_DIR)/app

clean:
	rm -rf $(OBJS) $(OBJS:.o=.d) $(LINKED_OBJS) $(LINKED_OBJS:.o=.d) $(BIN_DIR)/*
