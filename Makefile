# Makefile for Tiny Flood raylib game on Windows + MinGW-w64
# Adjust this to wherever you unzipped raylib
RAYLIB_DIR := C:/raylib
CXX     := g++
WINDRES := windres
CXXFLAGS := -std=c++17 -Wall -O2 -I$(RAYLIB_DIR)/include
LDFLAGS := -L$(RAYLIB_DIR)/lib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
SRC     := tinyflood.cpp
OBJ     := $(SRC:.cpp=.o)
RESOURCE_SRC := tinyflood.rc
RESOURCE_OBJ := tinyflood_res.o
TARGET  := tinyflood.exe

.PHONY: all clean run

all: $(TARGET)

# Link step (include resource object)
$(TARGET): $(OBJ) $(RESOURCE_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Compile C++ source
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Windows resource file
$(RESOURCE_OBJ): $(RESOURCE_SRC)
	$(WINDRES) $< -o $@

# Run the game
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(RESOURCE_OBJ) $(TARGET)
