#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "app.h"
#include "camera.h"
#include "model.h"
#include "shader.h"

typedef struct {
  Camera camera;
  Glsb_Shader shader;
  Texture tex;
  GLuint vao;
} Context;

static void startup(Glsb_App* app) {
  Context* ctx = app->context;

  ctx->tex = texture_init("asset/smile.png", TextureDiffuse);
  ctx->camera = camera_init(v3f(0,0,4));
  ctx->shader = glsb_shader_init("glsl/texture.vert", "glsl/texture.frag");

  GLfloat data[] = {
    // vertice x, y  texture s, t
    -1.0f,  1.0f,    0.0f, 1.0f,
    -1.0f, -1.0f,    0.0f, 0.0f,
     1.0f, -1.0f,    1.0f, 0.0f,
     1.0f, -1.0f,    1.0f, 0.0f,
     1.0f,  1.0f,    1.0f, 1.0f,
    -1.0f,  1.0f,    0.0f, 1.0f,
  };

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

  glGenVertexArrays(1, &ctx->vao);
  glBindVertexArray(ctx->vao);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), NULL);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),
                                                  (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);
}

static void render(Glsb_App* app, double curr_secs) {
  (void)curr_secs;

  Context* ctx = app->context;

  camera_update(&ctx->camera, &app->input, app->elapsed_secs);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  M4f view = camera_view(&ctx->camera);
  glsb_shader_use(ctx->shader);
  M4f model = m4f_id();
  glsb_shader_set_m4f(ctx->shader, "model", model);
  glsb_shader_set_m4f(ctx->shader, "view", view);
  glsb_shader_set_m4f(ctx->shader, "projection", app->projection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ctx->tex.id);
  glBindVertexArray(ctx->vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}


int main() {
  glsb_app_init();

  Context ctx = {0};
  glsb_app_set_context(&ctx);

  glsb_app_startup(startup);
  glsb_app_render(render);

  glsb_app_run();

  glsb_app_deinit();

  return 0;
}
