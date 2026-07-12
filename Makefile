CFLAGS = -Wall -Wextra -g -O2 -std=c11
LIBS = $(shell pkg-config --libs glfw3) -lm
SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c,build/obj/%.o,$(SOURCES))
THIRDPARTIES = build/obj/glad.o build/obj/stb_image.o
INCLUDES = -I./src -I./thirdparty/glad/include -I./thirdparty/stb

$(info SOURCES = $(SOURCES))
$(info OBJECTS = $(OBJECTS))

.PHONY: all
all: build/main build/tex

.PHONY: run/main
run/main: build/main
	./build/main

.PHONY: run/tex
run/tex: build/tex
	./build/tex

.PHONY: clean
clean:
	rm -rf build/*

build/main: cmd/main.c $(OBJECTS) $(THIRDPARTIES) | prepare
	gcc $(CFLAGS) -o $@ $^ $(INCLUDES) $(LIBS)

build/tex: cmd/tex.c $(OBJECTS) $(THIRDPARTIES) | prepare
	gcc $(CFLAGS) -o $@ $^ $(INCLUDES) $(LIBS)

build/obj/%.o: src/%.c | prepare
	gcc $(CFLAGS) -c -o $@ $< $(INCLUDES)

# thirdparty deps begin #################################################################
# glad
build/obj/glad.o: thirdparty/glad/src/gl.c | prepare
	gcc $(CFLAGS) -c -o $@ $< $(INCLUDES)

# stb_image
build/obj/stb_image.o: thirdparty/stb/stb_image.c | prepare
	gcc $(CFLAGS) -c -o $@ $< $(INCLUDES)

# thirdparty deps end   #################################################################

.PHONY: prepare
prepare:
	mkdir -p build/obj

.PHONY: lsp
lsp: clean
	bear -- make
