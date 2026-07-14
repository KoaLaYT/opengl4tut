#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <glad/gl.h>

#include "app.h"
#include "shader.h"

typedef struct {
  Glsb_Shader shader;
  GLuint vao;
} Context;

static void list_extensions();

static void startup(Glsb_App* app) {
  Context* ctx = app->context;

  ctx->shader = glsb_shader_init("glsl/hello.vert", "glsl/hello.frag");
  glGenVertexArrays(1, &ctx->vao);
  glBindVertexArray(ctx->vao);

  list_extensions();
}

static void render(Glsb_App* app, double current_time) {
  Context* ctx = app->context;

  GLfloat bg_color[] = { cos(current_time) * 0.5f + 0.5f,
                         sin(current_time) * 0.5f + 0.5f, 0.0f, 1.0f };
  GLfloat fg_color[] = { sin(current_time) * 0.5f + 0.5f,
                         cos(current_time) * 0.5f + 0.5f, 0.0f, 1.0f };
  glClearBufferfv(GL_COLOR, 0, bg_color);

  GLfloat offset[] = { cos(current_time) * 0.5f,
                       sin(current_time) * 0.6f, 0.0f, 1.0f };

  glsb_shader_use(ctx->shader);
  glVertexAttrib4fv(0, offset);
  glVertexAttrib4fv(1, fg_color);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void shutdown(Glsb_App* app) {
  Context* ctx = app->context;

  glsb_shader_deinit(ctx->shader);
  glDeleteVertexArrays(1, &ctx->vao);
}

int main() {
  glsb_app_init();

  Context ctx = {0};
  glsb_app_set_context(&ctx);

  glsb_app_startup(startup);
  glsb_app_render(render);
  glsb_app_shutdown(shutdown);

  glsb_app_run();

  glsb_app_deinit();

  return 0;
}

static void list_extensions() {
  GLint num;
  glGetIntegerv(GL_NUM_EXTENSIONS, &num);

  printf("Supported extensions: %d\n", num);
  for (GLint i = 0; i < num; i++) {
    const GLubyte* extname = glGetStringi(GL_EXTENSIONS, i);
    printf("%04d %s\n", i, extname);
  }
}
