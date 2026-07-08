CFLAGS = -Wall -Wextra -g -O2 -std=c11
LIBS = $(shell pkg-config --libs glfw3) -lm
SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c,build/%.o,$(SOURCES))
INCLUDES = -I./thirdparty/glad/include

$(info SOURCES = $(SOURCES))
$(info OBJECTS = $(OBJECTS))

.PHONY: all
all: build/main

.PHONY: run
run: build/main
	./build/main

.PHONY: clean
clean:
	rm -rf build/*

build/main: $(OBJECTS) build/glad.o | build
	gcc $(CFLAGS) -o build/main $(OBJECTS) build/glad.o $(INCLUDES) $(LIBS)

build/%.o: src/%.c | build
	gcc $(CFLAGS) -c -o $@ $< $(INCLUDES)

# thirdparty deps begin #################################################################
# glad
build/glad.o: thirdparty/glad/src/gl.c | build
	gcc $(CFLAGS) -c -o build/glad.o thirdparty/glad/src/gl.c $(INCLUDES)

# thirdparty deps end   #################################################################

build:
	mkdir -p build

.PHONY: lsp
lsp: clean
	bear -- make
