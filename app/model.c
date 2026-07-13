#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>

#include "tut.h"
#include "la.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

#define MOUSE_SENSITIVITY 0.05
#define MOVE_SPEED        6

static int g_win_width  = 1280;
static int g_win_height = 720;
static M4f g_projection = {0};
static Camera g_camera = {0};
static bool g_mouse_button_right_pressed = false;
static bool g_first_mouse = true;
static float g_last_xpos = 0;
static float g_last_ypos = 0;

static double update_fps_counter(GLFWwindow* window);
static void glfw_error_callback(int error, const char* description);
static void glfw_framebuffer_size_callback(GLFWwindow* win, int width, int height);
static void update_projection_matrix(int width, int height);
static void process_keypress(GLFWwindow* win, float elapsed_secs);
static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {
  tut_restart_log();

  tut_log_info("starting GLFW %s\n", glfwGetVersionString());

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    tut_log_err("could not start GLFW3\n");
    return 1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // anti-aliasing
#if __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
  GLFWwindow* window = glfwCreateWindow(g_win_width, g_win_height, "", NULL, NULL);
  if (!window) {
    tut_log_err("could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
  glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
  glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);

  gladLoadGL(glfwGetProcAddress);
  tut_log_glparams();

  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"
  // glEnable(GL_CULL_FACE);  // cull face
  // glCullFace(GL_BACK);     // cull back face
  // glFrontFace(GL_CW);      // clock-wise

#if __APPLE__
  {
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);
    update_projection_matrix(fb_width, fb_height);
  }
#endif

#if __APPLE__
  const char* model_path = "/Users/koalayt/Downloads/backpack/backpack.obj";
#else
  const char* model_path = "/home/koalayt/Downloads/backpack/backpack.obj";
#endif
  Model m = model_init(model_path);
  Glsb_Shader shader  = glsb_shader_init("glsl/model.vert", "glsl/model.frag");
  g_camera = camera_init(v3f(0,0,4));
  V3f light_color = v3f(1.0,1.0,1.0);
  V3f diffuse_color = v3f(light_color.x*0.5,light_color.y*0.5,light_color.z*0.5);
  V3f ambient_color = v3f(light_color.x*0.1,light_color.y*0.1,light_color.z*0.1);
  glsb_shader_use(shader);
  glsb_shader_set_v3f(shader, "dir_light.ambient", ambient_color);
  glsb_shader_set_v3f(shader, "dir_light.diffuse", diffuse_color);
  glsb_shader_set_v3f(shader, "dir_light.specular", light_color);
  glsb_shader_set_v3f(shader, "dir_light.direction", v3f(-0.2,-1.0,-0.3));
  glsb_shader_set_float(shader, "material.shininess", 32.0f);

  float rad = 0;

  while (!glfwWindowShouldClose(window)) {
    double elapsed_secs = update_fps_counter(window);

    rad += DEG_TO_RAD(10*elapsed_secs);

    process_keypress(window, elapsed_secs);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    M4f view = camera_view(&g_camera);
    glsb_shader_use(shader);
    M4f model = m4f_id();
    model = m4f_mul(model, m4f_rot_y(rad));
    glsb_shader_set_m4f(shader, "model", model);
    glsb_shader_set_m4f(shader, "view", view);
    glsb_shader_set_m4f(shader, "projection", g_projection);
    model_draw(&m, shader);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  model_deinit(&m);
  glfwTerminate();
  return 0;
}

static double update_fps_counter(GLFWwindow* window)
{
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

static void glfw_framebuffer_size_callback(GLFWwindow* win, int width, int height) {
  (void)win;
  glViewport(0, 0, width, height);
  update_projection_matrix(width, height);
}

static void update_projection_matrix(int w, int h) {
  M4f m = {0};
  float near = 0.1f;
  float far = 100.0f;
  float fov = DEG_TO_RAD(66);
  float aspect = (float)w / (float)h;
  float range = tan(fov/2) * near;
  float sx = near / (range*aspect);
  float sy = near/range;
  float sz = -(far+near)/(far-near);
  float pz = -(2*far*near)/(far-near);
  m._11 = sx;
  m._22 = sy;
  m._33 = sz; m._43 = pz;
  m._34 = -1;
  g_projection = m;
}

static void process_keypress(GLFWwindow* window, float elapsed_secs) {
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
    camera_forward(&g_camera, +MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
    camera_forward(&g_camera, -MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
    camera_right(&g_camera, -MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
    camera_right(&g_camera, +MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) {
    camera_up(&g_camera, +MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) {
    camera_up(&g_camera, -MOVE_SPEED*elapsed_secs);
  }
}

static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
  (void)window;

  if (g_mouse_button_right_pressed) {
    if (g_first_mouse) {
      g_last_xpos = xpos;
      g_last_ypos = ypos;
      g_first_mouse = false;
    }

    double dx = xpos - g_last_xpos;
    double dy = ypos - g_last_ypos;
    g_last_xpos = xpos;
    g_last_ypos = ypos;
    camera_yaw(&g_camera, -dx*MOUSE_SENSITIVITY);
    camera_pitch(&g_camera, -dy*MOUSE_SENSITIVITY);
  }
}

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  (void)window;
  (void)mods;

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    g_mouse_button_right_pressed = true;
  } else {
    g_mouse_button_right_pressed = false;
    g_first_mouse = true;
  }
}
