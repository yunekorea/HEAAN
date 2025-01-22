# This sample should be run after make libHEAAN.a file in lib folder

# All Target
all: clean ProfilingHEAAN

# Tool invocations
ExampleHEAAN:
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ Profiling.cpp -o ProfilingHEAAN -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O2 -I/../src -pthread
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	rm -rf ProfilingHEAAN
