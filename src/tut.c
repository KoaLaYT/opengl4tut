// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "tut.h"
#include <errno.h>
#include <error.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#define GL_LOG_FILE "/tmp/opengl4tut.log"
#define SHADER_LOG_SIZE 2048

const char* tut_read_entire_file(const char* path) {
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

void tut_restart_log() {
  FILE* f = fopen(GL_LOG_FILE, "w");
  if (!f) {
    error(1, errno, "tut_restart_log: fopen: %s", GL_LOG_FILE);
  }

  time_t now = time(NULL);
  char* date = ctime(&now);
  fprintf(f, "GL_LOG_FILE log. local time %s\n", date);
  fclose(f);
}

void tut_log_debug(const char* msg, ...) {
  fprintf(stdout, "[DEBUG] ");
  va_list ap;
  va_start(ap, msg);
  vfprintf(stdout, msg, ap);
  va_end(ap);
}

void tut_log_info(const char* msg, ...) {
  va_list ap;
  FILE* f = fopen(GL_LOG_FILE, "a");
  if (!f) {
    error(1, errno, "tut_log: fopen: %s", GL_LOG_FILE);
  }

  fprintf(f, "[INFO] ");
  va_start(ap, msg);
  vfprintf(f, msg, ap);
  va_end(ap);

  fclose(f);
}

void tut_log_err(const char* msg, ...) {
  va_list ap;
  FILE* f = fopen(GL_LOG_FILE, "a");
  if (!f) {
    error(1, errno, "tut_log_err: fopen: %s", GL_LOG_FILE);
  }

  fprintf(f, "[ERROR] ");
  va_start(ap, msg);
  vfprintf(f, msg, ap);
  va_end(ap);

  fprintf(stderr, "[ERROR] ");
  va_start(ap, msg);
  vfprintf(stderr, msg, ap);
  va_end(ap);

  fclose(f);
}

void tut_log_glparams() {
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  tut_log_info("Renderer: %s\n", renderer);
  tut_log_info("OpenGL version supported: %s\n", version);

  GLenum params[] = {
      GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
      GL_MAX_CUBE_MAP_TEXTURE_SIZE,
      GL_MAX_DRAW_BUFFERS,
      GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
      GL_MAX_TEXTURE_IMAGE_UNITS,
      GL_MAX_TEXTURE_SIZE,
      GL_MAX_VARYING_FLOATS,
      GL_MAX_VERTEX_ATTRIBS,
      GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
      GL_MAX_VERTEX_UNIFORM_COMPONENTS,
      GL_MAX_VIEWPORT_DIMS,
      GL_STEREO,
  };
  const char* names[] = {
      "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
      "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
      "GL_MAX_DRAW_BUFFERS",
      "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
      "GL_MAX_TEXTURE_IMAGE_UNITS",
      "GL_MAX_TEXTURE_SIZE",
      "GL_MAX_VARYING_FLOATS",
      "GL_MAX_VERTEX_ATTRIBS",
      "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
      "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
      "GL_MAX_VIEWPORT_DIMS",
      "GL_STEREO",
  };
  tut_log_info("GL Context Params:\n");
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv(params[i], &v);
    tut_log_info("%s %i\n", names[i], v);
  }
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv(params[10], v);
  tut_log_info("%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv(params[11], &s);
  tut_log_info("%s %u\n", names[11], (unsigned int)s);
  tut_log_info("-----------------------------\n");
}

GLuint tut_compile_program(const char* vs_path, const char* fs_path) {
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

  GLuint sp = glCreateProgram();
  glAttachShader(sp, vs);
  glAttachShader(sp, fs);
  glLinkProgram(sp);

  glGetProgramiv(sp, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
    tut_log_err("could not link shader program %i\n", sp);
    glGetProgramInfoLog(sp, SHADER_LOG_SIZE, &len, log);
    tut_log_debug("Program info log for GL index %u: %.*s\n", sp, len, log);
    exit(1);
  }

  return sp;
}
