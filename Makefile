# Define compiler and compiler flags
CXX := g++
CXXFLAGS := -I src/include

# Define linker flags
LDFLAGS := -L lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-main

# Define the output executable name
TARGET := my_project.exe

# Define the source files
SRC := main.cpp

# Define the objects files
OBJ := $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Rule to compile the source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
