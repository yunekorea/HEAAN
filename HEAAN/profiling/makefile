# This sample should be run after make libHEAAN.a file in lib folder

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -p -Wall -O2 -I../src -pthread #-g
HEAAN_LDFLAGS = -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -pthread
NDP_LDFLAGS = -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -pthread -lnvme

# Source and object files
HEAAN_SRCS = Profiling.cpp FileSave.cpp
NDP_SRCS = ProfilingNDP.cpp FileSave.cpp

#FileLoad.cpp
HEAAN_OBJS = $(HEAAN_SRCS:.cpp=.o)
NDP_OBJS = $(NDP_SRCS:.cpp=.o)
# Target executable name
TARGETS = ProfilingHEAAN ProfilingNDP

# Default rule
all: clean $(TARGETS)

# Linking step
ProfilingHEAAN: $(HEAAN_OBJS)
	@echo 'Building target: $@'
	$(CXX) $(CXXFLAGS) -o $@ $^ $(HEAAN_LDFLAGS)
	@echo 'Finished building target: $@'

ProfilingNDP: $(NDP_OBJS)
	@echo 'Building target: $@'
	$(CXX) $(CXXFLAGS) -o $@ $^ $(NDP_LDFLAGS)
	@echo 'Finished building target: $@'

# Compile individual source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(HEAAN_OBJS) $(NDP_OBJS) $(TARGETS)

