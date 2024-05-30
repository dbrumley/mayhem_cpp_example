# Compiler settings
CC = g++
CFLAGS = -std=c++11 -fno-omit-frame-pointer

# AFL and libFuzzer specific settings
AFL_CC = afl-g++
LIBFUZZER_CC = clang++
LIBFUZZER_FLAGS = -fsanitize=fuzzer,address -DLIBFUZZER_FUZZ

# Target names
BINARY_TARGET = driver
AFL_TARGET = driver_afl
LIBFUZZER_TARGET = driver_fuzzer

# Source files
SOURCES = driver.cpp
HEADERS = TrackingData.h UTCClock.h

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(BINARY_TARGET) #$(AFL_TARGET) $(LIBFUZZER_TARGET)

# Rule to build the standard binary target
$(BINARY_TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build the AFL target
$(AFL_TARGET): $(OBJECTS)
	$(AFL_CC) $(CFLAGS) -o $@ $^

# Rule to build the libFuzzer target
$(LIBFUZZER_TARGET): $(OBJECTS)
	$(LIBFUZZER_CC) $(CFLAGS) $(LIBFUZZER_FLAGS) -o $@ $^

# Rule to build object files
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(BINARY_TARGET) $(AFL_TARGET) $(LIBFUZZER_TARGET)

# Phony targets
.PHONY: all clean
