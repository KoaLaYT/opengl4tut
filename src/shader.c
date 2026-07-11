#include "shader.h"
#include "tut.h"
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#define SHADER_LOG_SIZE 2048

static const char* tut_read_entire_file(const char* path);

Shader shader_init(const char *vs_path, const char *fs_path) {
  int len = 0;
  char log[SHADER_LOG_SIZE];

  const char* vertex_shader = tut_read_entire_file(vs_path);
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  free((void*)vertex_shader);

  int params = -1;
  glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    tut_log_err("GL shader index %i did not compile\n", vs);
    glGetShaderInfoLog(vs, SHADER_LOG_SIZE, &len, log);
    tut_log_debug("Shader info log for GL index %u: %.*s\n", vs, len, log);
    exit(1);
  }

  const char* fragment_shader = tut_read_entire_file(fs_path);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);
  free((void*)fragment_shader);

  glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    tut_log_err("GL shader index %i did not compile\n", fs);
    glGetShaderInfoLog(fs, SHADER_LOG_SIZE, &len, log);
    tut_log_debug("Shader info log for GL index %u: %.*s\n", fs, len, log);
    exit(1);
  }

  GLuint id = glCreateProgram();
  glAttachShader(id, vs);
  glAttachShader(id, fs);
  glLinkProgram(id);

  glGetProgramiv(id, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
    tut_log_err("could not link shader program %i\n", id);
    glGetProgramInfoLog(id, SHADER_LOG_SIZE, &len, log);
    tut_log_debug("Program info log for GL index %u: %.*s\n", id, len, log);
    exit(1);
  }

  return (Shader){ .id = id };
}

void shader_use(Shader s) {
  glUseProgram(s.id);
}

void shader_set_m4f(Shader s, const char* name, M4f value) {
  int loc = glGetUniformLocation(s.id, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, value.a);
}

void shader_set_v3f(Shader s, const char* name, V3f value) {
  int loc = glGetUniformLocation(s.id, name);
  glUniform3fv(loc, 1, value.a);
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
  error(1, errno, "read_entire_file(%s)", path);
  return NULL;
}
