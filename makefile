# Compiler & flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = build
TARGET = $(OBJ_DIR)/main

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule for building object files into build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
server: $(TARGET)
	./$(TARGET) server

client: $(TARGET)
	./$(TARGET) client

# Clean build files
clean:
	rm -rf $(OBJ_DIR)
