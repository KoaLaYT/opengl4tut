#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <error.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "la.h"
#include "tut.h"

#define PI 3.14159f
#define DEG_TO_RAD(deg) (deg*PI/180.0f)

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

typedef struct {
  V3f pos;
  float yaw, pitch; // deg
  bool need_update;
  M4f cached_view;
} camera;

static camera camera_init(V3f at) {
  camera c = {0};
  c.pos = at;
  c.need_update = true;
  return c;
}

static M4f camera_view(camera* c) {
  if (c->need_update) {
    Quat yaw = quat_init(DEG_TO_RAD(c->yaw), v3f(0,1,0));
    Quat pitch = quat_init(DEG_TO_RAD(c->pitch), v3f(1,0,0));
    Quat q = quat_mul(yaw, pitch);
    Quat q_inv = quat_conjugate(q);

    M4f t = m4f_translate(v3f_neg(c->pos));
    M4f r = quat_to_m4f(q_inv);
    c->cached_view = m4f_mul(r, t);
    c->need_update = false;
  }
  return c->cached_view;
}

static void camera_yaw(camera* c, float deg) {
  c->need_update = true;
  c->yaw += deg;
}

static void camera_pitch(camera* c, float deg) {
  c->need_update = true;
  c->pitch += deg;
  if (c->pitch >  89) c->pitch =  89;
  if (c->pitch < -89) c->pitch = -89;
}

static M4f projection_matrix() {
  M4f m = {0};
  float near = 0.1f;
  float far = 100.0f;
  float fov = DEG_TO_RAD(66);
  float aspect = (float)g_fb_width / (float)g_fb_height;
  float range = tan(fov/2) * near;
  float sx = near / (range*aspect);
  float sy = near/range;
  float sz = -(far+near)/(far-near);
  float pz = -(2*far*near)/(far-near);
  m._11 = sx;
  m._22 = sy;
  m._33 = sz; m._43 = pz;
  m._34 = -1;
  return m;
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


  GLuint sp = tut_compile_program("glsl/hello.vert", "glsl/hello.frag");
  int model_loc      = glGetUniformLocation(sp, "model");
  int view_loc       = glGetUniformLocation(sp, "view");
  int projection_loc = glGetUniformLocation(sp, "projection");

  camera c = camera_init(v3f(0,0,5));
  float rad = 0.0f;
  float speed = 0.5f;
  V3f move = {0};

  while (!glfwWindowShouldClose(window)) {
     double elapsed_secs = update_fps_counter(window);

     if (fabs(move.x) > 1.0f) {
       speed = -speed;
     }
     rad += 2*elapsed_secs;
     if (rad > 2*PI) rad -= 2*PI;
     move.x += elapsed_secs * speed;

     M4f model = m4f_mul(m4f_translate(move), m4f_rot_z(rad));
     M4f view = camera_view(&c);
     M4f projection = projection_matrix();

     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glViewport(0, 0, g_fb_width, g_fb_height);
     glUseProgram(sp);
     glUniformMatrix4fv(model_loc, 1, GL_FALSE, model.a);
     glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.a);
     glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection.a);
     glBindVertexArray(vao);
     glDrawArrays(GL_TRIANGLES, 0, 3);
     glfwPollEvents();
     glfwSwapBuffers(window);

     if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, 1);
     }
     if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT)) {
        camera_yaw(&c, +1);
     }
     if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT)) {
        camera_yaw(&c, -1);
     }
     if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) {
        camera_pitch(&c, +1);
     }
     if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) {
        camera_pitch(&c, -1);
     }
  }

  glfwTerminate();
  return 0;
}
