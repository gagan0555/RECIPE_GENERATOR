CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# List of source files for the backend
SRC = backend_api.c recipe_manager.c file_storage.c recipe_ranking.c ingredient_substitution.c ingredient_map_fixed.c

# Object files generated from source
OBJ = $(SRC:.c=.o)

# Target executable name
TARGET = backend_api

.PHONY: all clean run

# Default target: build the executable
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

# Run the executable
run: $(TARGET
