CFLAGS = -Wall -Wextra -g -O2
LIB_GLFW = $(shell pkg-config --libs glfw3)
INCLUDE_GLAD = -I./thirdparty/glad/include

.PHONY: all
all: build/main

.PHONY: run
run: build/main
	./build/main

.PHONY: clean
clean:
	rm -rf build/*

build/main: src/main.c build/glad.o | build
	gcc $(CFLAGS) -o build/main src/main.c build/glad.o $(INCLUDE_GLAD) $(LIB_GLFW)

build/glad.o: thirdparty/glad/src/gl.c | build
	gcc $(CFLAGS) -c -o build/glad.o thirdparty/glad/src/gl.c $(INCLUDE_GLAD)

build:
	mkdir -p build

.PHONY: lsp
lsp: clean
	bear -- make
