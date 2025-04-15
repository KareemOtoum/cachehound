CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
TARGET = build/main

SRCS = main.cpp LRUCache.cpp
OBJS = $(SRCS:%.cpp=build/%.o)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to build/*.o
build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
build:
	mkdir -p build

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up all build files
clean:
	rm -rf build
