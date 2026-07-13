#include <glad/gl.h>

#include "app.h"

void render(Glsb_App* app, double current_time) {
  static const GLfloat red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
  glClearBufferfv(GL_COLOR, 0, red);
}

int main() {
  Glsb_App* app = glsb_app_init();

  app->render = render;

  glsb_app_run();
  glsb_app_deinit();
  return 0;
}
