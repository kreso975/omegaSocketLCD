CCFLAGS=-c -O2 -fPIC
LDFLAGS=-lpthread -ldl

# build omegaSocketLCD executable when user executes "make"
SOURCES=$(shell find -name "*.cpp" | sed s:./:: | tr "\n" " ")
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

# Includes and libraries
INC=-I ./headers

# Main build target
TARGET=omegaSocketLCD

# Default target - build everything
all: precomp $(TARGET)

# Link target
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

# Compile sources 
build/%.o: ./sources/%.cpp
	$(CXX) $(CCFLAGS) $(INC) $< -o $@

# Remove all created files
clean:
	rm -rf build/*
	rm -rf dist/*

# Ensure required sub-directories are created
precomp:
	@mkdir -p build
	@mkdir -p dist