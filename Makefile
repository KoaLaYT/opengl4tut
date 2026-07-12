CFLAGS = -Wall -Wextra -g -O2 -std=c11
LIBS = $(shell pkg-config --libs glfw3) -lm
SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c,build/obj/%.o,$(SOURCES))
THIRDPARTIES = build/obj/glad.o
INCLUDES = -I./thirdparty/glad/include -I./src

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

build/main: $(OBJECTS) build/obj/glad.o | prepare
	gcc $(CFLAGS) -o build/main cmd/main.c $(OBJECTS) $(THIRDPARTIES) $(INCLUDES) $(LIBS)

build/obj/%.o: src/%.c | prepare
	gcc $(CFLAGS) -c -o $@ $< $(INCLUDES)

# thirdparty deps begin #################################################################
# glad
build/obj/glad.o: thirdparty/glad/src/gl.c | prepare
	gcc $(CFLAGS) -c -o build/obj/glad.o thirdparty/glad/src/gl.c $(INCLUDES)

# thirdparty deps end   #################################################################

.PHONY: prepare
prepare:
	mkdir -p build/obj

.PHONY: lsp
lsp: clean
	bear -- make
