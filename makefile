---

**Makefile**

```makefile
# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Ilib/chess/include -I./include

# Directories
SRC_DIR = src
BUILD_DIR = build
LIB_CHESS_DIR = lib/chess
LIB_CHESS_SRC = $(LIB_CHESS_DIR)/src/chess.c
LIB_CHESS_BUILD_DIR = $(LIB_CHESS_DIR)/build
LIB_CHESS = $(LIB_CHESS_BUILD_DIR)/libchess.a

# Source files for main application (assumed to be in src/)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Final executable name
TARGET = chessbot

.PHONY: all clean

all: $(TARGET)

# Build the chess library as a static library
$(LIB_CHESS): $(LIB_CHESS_SRC)
	@mkdir -p $(LIB_CHESS_BUILD_DIR)
	$(CC) $(CFLAGS) -c $(LIB_CHESS_SRC) -o $(LIB_CHESS_BUILD_DIR)/chess.o
	ar rcs $(LIB_CHESS) $(LIB_CHESS_BUILD_DIR)/chess.o

# Build main application objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the final executable
$(TARGET): $(OBJS) $(LIB_CHESS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB_CHESS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(LIB_CHESS_BUILD_DIR) $(TARGET)
