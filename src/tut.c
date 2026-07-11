#include "tut.h"
#include <glad/gl.h>
#include <errno.h>
#include <error.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define GL_LOG_FILE "/tmp/opengl4tut.log"

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
  fflush(stdout);
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
  fclose(f);

  fprintf(stderr, "[ERROR] ");
  va_start(ap, msg);
  vfprintf(stderr, msg, ap);
  va_end(ap);
  fflush(stderr);
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
