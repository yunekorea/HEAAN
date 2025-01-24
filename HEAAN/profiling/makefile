# This sample should be run after make libHEAAN.a file in lib folder

# All Target
all: clean ProfilingHEAAN

# Tool invocations
ProfilingHEAAN: FileSave.o FileLoad.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ Profiling.cpp -o ProfilingHEAAN -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O2 -I/../src -pthread
	@echo 'Finished building target: $@'
	@echo ' '

FileSave.o : FileSave.h
	g++ -c -g FileSave.cpp

FileLoad.o : FileLoad.h
	g++ -c -g FileLoad.cpp

# Other Targets
clean:
	rm -rf ProfilingHEAAN
