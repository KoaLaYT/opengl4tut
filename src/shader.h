#ifndef INCLUDE_SHADER_H
#define INCLUDE_SHADER_H

#include <glad/gl.h>

#include "la.h"

typedef struct {
  GLuint id;
} Glsb_Shader;

Glsb_Shader glsb_shader_init(const char* vs_path, const char* fs_path);
void glsb_shader_deinit(Glsb_Shader s);
void glsb_shader_use(Glsb_Shader s);
void glsb_shader_set_int(Glsb_Shader s, const char* name, int value);
void glsb_shader_set_float(Glsb_Shader s, const char* name, float value);
void glsb_shader_set_v3f(Glsb_Shader s, const char* name, V3f value);
void glsb_shader_set_m4f(Glsb_Shader s, const char* name, M4f value);

#endif // INCLUDE_SHADER_H
