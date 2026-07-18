CFLAGS = -Wall -Wextra -g -O2 -std=c11
CC = gcc
LIBS = $(shell pkg-config --libs glfw3) \
			 -lm \
			 -Wl,-rpath,'$$ORIGIN/../lib' -L./thirdparty/assimp -lassimp
APP_SRCS = $(wildcard app/*.c)
APPS     = $(patsubst app/%.c,build/bin/%,$(APP_SRCS))
SOURCES  = $(wildcard src/*.c)
OBJECTS  = $(patsubst src/%.c,build/obj/%.o,$(SOURCES))
THIRDPARTIES = build/obj/glad.o build/obj/stb_image.o
INCLUDES = -I./src \
					 -I./thirdparty/glad/include \
					 -I./thirdparty/stb \
					 -I./thirdparty/assimp/include

$(info SOURCES = $(SOURCES))
$(info OBJECTS = $(OBJECTS))
$(info APPS    = $(APPS))

.PHONY: all
all: $(APPS)

.PHONY: clean
clean:
	rm -rf build/*

.PHONY: run/%
run/%: build/bin/%
	./build/bin/$*

build/bin/%: app/%.c $(OBJECTS) $(THIRDPARTIES) | prepare
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDES) $(LIBS)

build/obj/%.o: src/%.c | prepare
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES)

# thirdparty deps begin #################################################################
# glad
build/obj/glad.o: thirdparty/glad/src/gl.c | prepare
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES)

# stb_image
build/obj/stb_image.o: thirdparty/stb/stb_image.c | prepare
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES)

# assimp
# manual build, see README.md

# thirdparty deps end   #################################################################

.PHONY: prepare
prepare:
	mkdir -p build/obj build/bin build/lib
	cp -a thirdparty/assimp/libassimp.* build/lib

.PHONY: lsp
lsp: clean
	bear -- make
