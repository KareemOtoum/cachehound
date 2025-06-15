CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O3 -march=native -Iinclude

SRC_DIR = src
OBJ_DIR = build
TARGET = $(OBJ_DIR)/main

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, \
        $(filter-out $(SRC_DIR)/throughput_test.cpp, $(SRCS)))

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

server: $(TARGET)
	./$(TARGET) server $(capacity)

client: $(TARGET)
	./$(TARGET) client $(ip) $(port)

clean:
	rm -rf $(OBJ_DIR)
