SHELL = /bin/bash
CC = g++
CPPFLAGS = -O3 -DNDEBUG -funroll-loops -static -Wall -Wextra -Wshadow -pedantic
OPTIONS = -O3 -DNDEBUG -funroll-loops -static -Wall -Wextra -Wshadow -pedantic -std=c++17
INCLUDES = -Iinclude

# SDSL configuration - adjust these paths if needed
SDSL_INCLUDE = sdsl-lite/include
SDSL_LIB = sdsl-lite/lib

# Ensure build directory exists
BUILD_DIR = build
$(shell mkdir -p $(BUILD_DIR))

# Targets
all: $(BUILD_DIR)/count $(BUILD_DIR)/gensa

debug: OPTIONS = -O0 -g -DDEBUG -Wall -Wextra -Wshadow -pedantic
debug: all

# Build SDSL if not present
sdsl:
	@if [ ! -d "sdsl-lite" ]; then \
		echo "Cloning SDSL-lite repository..."; \
		git clone https://github.com/simongog/sdsl-lite.git; \
	fi
	@if [ ! -f "$(SDSL_LIB)/libsdsl.a" ]; then \
		echo "Building SDSL-lite..."; \
		cd sdsl-lite && ./install.sh $(PWD)/sdsl-lite; \
	fi

# Build count with SDSL support
$(BUILD_DIR)/count: sdsl src/count.cpp include/common.cpp include/kkp.cpp | $(BUILD_DIR)
	$(CC) $(OPTIONS) $(INCLUDES) -I$(SDSL_INCLUDE) \
		-o $@ src/count.cpp include/common.cpp include/kkp.cpp \
		-L$(SDSL_LIB) -lsdsl -ldivsufsort -ldivsufsort64

# Build gensa (no SDSL needed for this one)
$(BUILD_DIR)/gensa: src/gensa.cpp include/common.cpp include/divsufsort.c | $(BUILD_DIR)
	$(CC) $(OPTIONS) $(INCLUDES) -o $@ src/gensa.cpp include/common.cpp include/divsufsort.c

# Phony targets
.PHONY: all debug clean sdsl

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)/*
	
# Optional: deep clean including SDSL library
distclean: clean
	rm -rf sdsl-lite
