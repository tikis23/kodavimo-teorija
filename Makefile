# be careful using when using this makefile
# you should 'make clean' after modifying headers

CC = g++
CFLAGS = -std=c++23 -Wall -Wextra -Wpedantic
LFLAGS = -lstdc++exp
OUT = program
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)

build:
	mkdir build

release: CFLAGS += -O3 -DNDEBUG
release: $(OUT)
debug: CFLAGS += -g
debug: $(OUT)

# link final program
$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

# compile obj
build/%.o: src/%.cpp | build
	$(CC) $(CFLAGS) -c $< -o $@

ifeq ($(OS),Windows_NT)
    CLEAN_CMD = rmdir /S /Q
else
    CLEAN_CMD = rm -rf
endif
clean:
	$(CLEAN_CMD) build