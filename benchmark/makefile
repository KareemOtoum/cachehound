CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I../include

SRC_DIR = ../src
OBJ_DIR = ../build
TARGET = $(OBJ_DIR)/throughput

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, \
        $(filter-out $(SRC_DIR)/main.cpp, $(SRCS)))

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

benchmark: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)
