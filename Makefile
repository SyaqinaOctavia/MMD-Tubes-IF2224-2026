CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Isrc
BUILD_DIR := build
SRC_DIR := src
TARGET := compiler
ARGS :=

CPP_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
CPP_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SOURCES))
DEPFILES := $(CPP_OBJECTS:.o=.d)

.PHONY: all objects run clean help

all: $(TARGET)

help:
	@echo "Targets:"
	@echo "  make          Compile all object files and link $(TARGET)"
	@echo "  make objects  Compile only object files into $(BUILD_DIR)/"
	@echo "  make run      Build and run $(TARGET); pass args with ARGS=\"...\""
	@echo "  make clean    Remove build outputs"

objects: $(CPP_OBJECTS)

$(TARGET): $(CPP_OBJECTS)
	$(CXX) $(CPP_OBJECTS) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPFILES)
