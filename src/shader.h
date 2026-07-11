#ifndef INCLUDE_SHADER_H
#define INCLUDE_SHADER_H

#include "glad/gl.h"
#include "la.h"

typedef struct {
  GLuint id;
} Shader;

Shader shader_init(const char* vs_path, const char* fs_path);
void shader_use(Shader s);
void shader_set_v3f(Shader s, const char* name, V3f value);
void shader_set_m4f(Shader s, const char* name, M4f value);

#endif // INCLUDE_SHADER_H
