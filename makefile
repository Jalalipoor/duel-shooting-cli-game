CXX = g++
CXXFLAGS = -std=c++20 -Iinclude
TARGET = UTGame
OBJ_DIR = obj

SRCS = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJS = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean