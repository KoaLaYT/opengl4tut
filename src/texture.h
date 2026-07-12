#ifndef INCLUDE_TEXTURE_H
#define INCLUDE_TEXTURE_H

#include <glad/gl.h>

typedef struct {
  GLuint id;
} Texture;

Texture texture_init(const char* file_path);

#endif // INCLUDE_TEXTURE_H
