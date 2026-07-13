#include <assert.h>
#include <math.h>

#include <glad/gl.h>

#include "app.h"
#include "shader.h"

typedef struct {
  Glsb_Shader shader;
  GLuint vao;
} Context;

void startup(Glsb_App* app) {
  Context* ctx = app->context;

  ctx->shader = glsb_shader_init("glsl/hello.vert", "glsl/hello.frag");
  glGenVertexArrays(1, &ctx->vao);
  glBindVertexArray(ctx->vao);
}

void render(Glsb_App* app, double current_time) {
  Context* ctx = app->context;

  GLfloat color[] = { cos(current_time) * 0.5f + 0.5f, 
                      sin(current_time) * 0.5f + 0.5f, 0.0f, 1.0f };
  glClearBufferfv(GL_COLOR, 0, color);

  GLfloat offset[] = { cos(current_time) * 0.5f,
                       sin(current_time) * 0.6f, 0.0f, 1.0f };

  glsb_shader_use(ctx->shader);
  glVertexAttrib4fv(0, offset);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void shutdown(Glsb_App* app) {
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
