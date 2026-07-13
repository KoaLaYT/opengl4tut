#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "shader.h"
#include "model.h"
#include "tut.h"

#define SHADER_LOG_SIZE 2048

static const char* tut_read_entire_file(const char* path);
static GLuint compile_shader(GLenum type, const char* path);

Glsb_Shader glsb_shader_init(const char *vs_path, const char *fs_path) {
  GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_path);
  GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_path);

  GLuint id = glCreateProgram();
  glAttachShader(id, vs);
  glAttachShader(id, fs);
  glLinkProgram(id);

  GLint ok = -1;
  glGetProgramiv(id, GL_LINK_STATUS, &ok);
  if (GL_TRUE != ok) {
    int len = 0;
    char log[SHADER_LOG_SIZE];
    glGetProgramInfoLog(id, SHADER_LOG_SIZE, &len, log);
    tut_log_err("Could not link shader %u, %.*s\n", id, len, log);
    exit(1);
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  return (Glsb_Shader){ .id = id };
}

void glsb_shader_deinit(Glsb_Shader s) {
  glUseProgram(0);
  glDeleteProgram(s.id);
}

void glsb_shader_use(Glsb_Shader s) {
  glUseProgram(s.id);
}

void glsb_shader_set_int(Glsb_Shader s, const char* name, int value) {
  int loc = glGetUniformLocation(s.id, name);
  glUniform1i(loc, value);
}

void glsb_shader_set_float(Glsb_Shader s, const char* name, float value) {
  int loc = glGetUniformLocation(s.id, name);
  glUniform1f(loc, value);
}

void glsb_shader_set_v3f(Glsb_Shader s, const char* name, V3f value) {
  int loc = glGetUniformLocation(s.id, name);
  glUniform3fv(loc, 1, value.a);
}

void glsb_shader_set_m4f(Glsb_Shader s, const char* name, M4f value) {
  int loc = glGetUniformLocation(s.id, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, value.a);
}

static const char* tut_read_entire_file(const char* path) {
  FILE* f = NULL;
  char* buf = NULL;

  struct stat s;
  if (stat(path, &s) != 0) {
    goto on_error;
  }
  size_t size = sizeof(char) * s.st_size + 1;

  f = fopen(path, "rb");
  if (f == NULL) {
    goto on_error;
  }

  buf = (char*)malloc(size);
  if (buf == NULL) {
    goto on_error;
  }

  size_t n = fread(buf, sizeof(char), size - 1, f);
  if (n != size - 1) {
    goto on_error;
  }

  buf[size - 1] = '\0';
  fclose(f);
  return buf;

on_error:
  if (f) {
    fclose(f);
  }
  if (buf) {
    free(buf);
  }
  tut_log_err("read_entire_file(%s): %s", path, strerror(errno));
  exit(1);
  return NULL;
}

static GLuint compile_shader(GLenum type, const char* path) {
  const char* code = tut_read_entire_file(path);
  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &code, NULL);
  glCompileShader(id);
  free((void*)code);

  GLint ok = -1;
  glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
  if (GL_TRUE != ok) {
    int len = 0;
    char log[SHADER_LOG_SIZE];
    glGetShaderInfoLog(id, SHADER_LOG_SIZE, &len, log);
    tut_log_err("Could not compile shader %u (%s), %.*s\n", id, path, len, log);
    exit(1);
  }

  return id;
}
