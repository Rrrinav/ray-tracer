# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

# Directories
SRC_DIR = src
OUTPUT_DIR = bin
OUTPUT_FILE ?= image.ppm

# Target executable name
TARGET ?= raytracer

# Build the project
all: $(OUTPUT_DIR)/$(TARGET)

# Compile the project
$(OUTPUT_DIR)/$(TARGET): $(SRC_DIR)/main.cpp
	@mkdir -p $(OUTPUT_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# Run the executable
run: $(OUTPUT_DIR)/$(TARGET)
	./$(OUTPUT_DIR)/$(TARGET)

# Generate image output
render: $(OUTPUT_DIR)/$(TARGET)
	./$(OUTPUT_DIR)/$(TARGET) > $(OUTPUT_FILE)

# Clean build artifacts
clean:
	rm -rf $(OUTPUT_DIR)

# Phony targets
.PHONY: all run render clean
