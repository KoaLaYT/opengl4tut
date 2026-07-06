#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <error.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "tut.h"

int g_win_width  = 640;
int g_win_height = 480;
int g_fb_width   = 640;
int g_fb_height  = 480;

static double update_fps_counter(GLFWwindow* window) {
  static double prev_update_sec = 0;
  static double prev_real_sec = 0;
  static int frame_count;

  double curr_sec = glfwGetTime();
  double elapsed_real_sec = curr_sec - prev_real_sec;
  double elapsed_update_sec = curr_sec - prev_update_sec;
  prev_real_sec = curr_sec;

  if (elapsed_update_sec > 0.25) {
    char tmp[128];
    double fps = (double)frame_count / elapsed_update_sec;
    sprintf(tmp, "opengl @ fps: %.2f", fps);
    glfwSetWindowTitle(window, tmp);
    frame_count = 0;
    prev_update_sec = curr_sec;
  } else {
    frame_count += 1;
  }

  return elapsed_real_sec;
}

static void glfw_error_callback(int error, const char* description) {
  tut_log_err("GLFW code: %i, description: %s\n", error, description);
}

static void glfw_window_size_callback(GLFWwindow* win, int width, int height) {
  (void)win;

  g_win_width = width;
  g_win_height = height;
}

static void glfw_framebuffer_size_callback(GLFWwindow* win, int width, int height) {
  (void)win;

  g_fb_width = width;
  g_fb_height = height;
}

int main(void) {
  tut_restart_log();

  tut_log_info("starting GLFW %s\n", glfwGetVersionString());

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    tut_log_err("could not start GLFW3\n");
    return 1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // anti-aliasing
  GLFWwindow* window = glfwCreateWindow(g_fb_width, g_fb_height, "", NULL, NULL);
  if (!window) {
    tut_log_err("could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

  gladLoadGL(glfwGetProcAddress);
  tut_log_glparams();

  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"
  glEnable(GL_CULL_FACE);  // cull face
  glCullFace(GL_BACK);     // cull back face
  glFrontFace(GL_CW);      // clock-wise

  GLfloat points[] = {
       0.0f,  0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
  };

  GLfloat colors[] = {
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f,
  };

  GLuint points_vbo = 0;
  glGenBuffers(1, &points_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

  GLuint colors_vbo = 0;
  glGenBuffers(1, &colors_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  // column major
  float matrix[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };

  GLuint sp = tut_compile_program("glsl/hello.vert", "glsl/hello.frag");
  int matrix_loc = glGetUniformLocation(sp, "matrix");

  float speed = 0.5f;
  float last_position = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    double elapsed_secs = update_fps_counter(window);

    if (fabs(last_position) > 1.0f) {
      speed = -speed;
    }
    last_position += elapsed_secs * speed;
    matrix[12] = last_position;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, g_fb_width, g_fb_height);
    glUseProgram(sp);
    glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, matrix);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwPollEvents();
    glfwSwapBuffers(window);

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
  }

  glfwTerminate();
  return 0;
}
