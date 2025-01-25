# This sample should be run after make libHEAAN.a file in lib folder

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -g -Wall -O2 -I../src -pthread
LDFLAGS = -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -pthread

# Source and object files
SRCS = Profiling.cpp FileSave.cpp FileLoad.cpp
OBJS = $(SRCS:.cpp=.o)

# Target executable name
TARGET = ProfilingHEAAN

# Default rule
all: clean $(TARGET)

# Linking step
$(TARGET): $(OBJS)
	@echo 'Building target: $@'
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo 'Finished building target: $@'

# Compile individual source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(OBJS) $(TARGET)

