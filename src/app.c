#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/gl.h>
#include "GLFW/glfw3.h"

#include "app.h"

#define UNUSED(s) (void)(s)

static void glsb_app_setup_default(Glsb_App* app);
static void glfw_on_window_resize(GLFWwindow* window, int w, int h);
static void glfw_on_framebuffer_resize(GLFWwindow* window, int w, int h);
static void glfw_on_key(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods);
static void glfw_on_mouse_button(GLFWwindow* window,
                                 int button,
                                 int action,
                                 int mods);
static void glfw_on_mouse_move(GLFWwindow* window, double x, double y);
static void glfw_on_mouse_wheel(GLFWwindow* window,
                                double xoffset,
                                double yoffset);

static void update_fps_counter(Glsb_App* app, double curr_secs);
static M4f update_projection_matrix(int w, int h);

static Glsb_App* g_app = NULL;

void glsb_app_init() {
  g_app = malloc(sizeof(Glsb_App));
  if (!g_app) {
    fprintf(stderr, "Buy more RAM!\n");
    return;
  }
  g_app->setup = glsb_app_setup_default;
}

void glsb_app_deinit() {
  if (g_app) free(g_app);
}

void glsb_app_run() {
  bool running = true;

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return;
  }

  assert(g_app->setup);
  g_app->setup(g_app);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, g_app->info.major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, g_app->info.minor_version);

  if (g_app->info.flags.debug) {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  }
  if (g_app->info.flags.robust) {
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
  }
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, g_app->info.samples);
  glfwWindowHint(GLFW_STEREO, g_app->info.flags.stereo ? GL_TRUE : GL_FALSE);

  g_app->window = glfwCreateWindow(g_app->info.window_width,
                                   g_app->info.window_height,
                                   "glsb",
                                   g_app->info.flags.fullscreen
                                     ? glfwGetPrimaryMonitor()
                                     : NULL,
                                   NULL);
  if (!g_app->window) {
    fprintf(stderr, "Failed to open window\n");
    return;
  }

  glfwMakeContextCurrent(g_app->window);
  glfwSetWindowSizeCallback(g_app->window, glfw_on_window_resize);
  glfwSetFramebufferSizeCallback(g_app->window, glfw_on_framebuffer_resize);
  glfwSetKeyCallback(g_app->window, glfw_on_key);
  glfwSetMouseButtonCallback(g_app->window, glfw_on_mouse_button);
  glfwSetCursorPosCallback(g_app->window, glfw_on_mouse_move);
  glfwSetScrollCallback(g_app->window, glfw_on_mouse_wheel);
  if (!g_app->info.flags.cursor) {
    glfwSetInputMode(g_app->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }

  gladLoadGL(glfwGetProcAddress);

  fprintf(stderr, "VENDOR  : %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "VERSION : %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "RENDERER: %s\n", glGetString(GL_RENDERER));

  // if (app->info.flags.debug) {
  //   if(gl3wIsSupported(4, 3)) {
  //     glDebugMessageCallback((GLDEBUGPROC)debug_callback, this);
  //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  //   }
  //   else if (sb6IsExtensionSupported("GL_ARB_debug_output")) {
  //     glDebugMessageCallbackARB((GLDEBUGPROC)debug_callback, this);
  //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
  //   }
  // }

  // initialize frame buffer size and update projection, just in case
  {
    int fb_width, fb_height;
    glfwGetFramebufferSize(g_app->window, &fb_width, &fb_height);
    glfw_on_framebuffer_resize(g_app->window, fb_width, fb_height);
  }

  if (g_app->startup) g_app->startup(g_app);

  do {
    double curr_secs = glfwGetTime();
    update_fps_counter(g_app, curr_secs);
    if (g_app->render) g_app->render(g_app, curr_secs);

    g_app->input.mouse_dx = 0;
    g_app->input.mouse_dy = 0;
    g_app->input.scroll_dy = 0;

    glfwSwapBuffers(g_app->window);
    glfwPollEvents();

    running &= (glfwGetKey(g_app->window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
    running &= (glfwWindowShouldClose(g_app->window) != GL_TRUE);
  } while (running);

  if (g_app->shutdown) g_app->shutdown(g_app);

  glfwDestroyWindow(g_app->window);
  glfwTerminate();
}

void glsb_app_set_context(void* context) {
  g_app->context = context;
}

void glsb_app_setup(Glsb_AppSetupFn* fn) {
  g_app->setup = fn;
}

void glsb_app_startup(Glsb_AppStartupFn* fn) {
  g_app->startup = fn;
}

void glsb_app_render(Glsb_AppRenderFn* fn) {
  g_app->render = fn;
}

void glsb_app_shutdown(Glsb_AppShutdownFn* fn) {
  g_app->shutdown = fn;
}

static void glsb_app_setup_default(Glsb_App* app) {
  app->info.window_width  = 800;
  app->info.window_height = 600;
  app->info.major_version = 4;
  app->info.minor_version = 6;
  app->info.samples = 0;
  app->info.flags.all = 0;
  app->info.flags.cursor = 1;
  app->info.flags.debug = 1;
}

static void glfw_on_framebuffer_resize(GLFWwindow* window, int w, int h) {
  UNUSED(window);
  glViewport(0, 0, w, h);
  g_app->projection = update_projection_matrix(w, h);
}

static void glfw_on_window_resize(GLFWwindow* window, int w, int h) {
  UNUSED(window);
  g_app->info.window_width = w;
  g_app->info.window_height = h;
}

static void glfw_on_key(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods)
{
  UNUSED(window); UNUSED(scancode); UNUSED(mods);
  if (key == GLFW_KEY_UNKNOWN) return;
  if (action == GLFW_PRESS) {
    g_app->input.keys[key] = true;
  } else if (action == GLFW_RELEASE) {
    g_app->input.keys[key] = false;
  }
}

static void glfw_on_mouse_button(GLFWwindow* window,
                                 int button,
                                 int action,
                                 int mods)
{
  UNUSED(window); UNUSED(mods);
  if (action == GLFW_PRESS) {
    g_app->input.mouse_buttons[button] = true;
  } else if (action == GLFW_RELEASE) {
    g_app->input.mouse_buttons[button] = false;
  }
}

static void glfw_on_mouse_move(GLFWwindow* window, double x, double y) {
  UNUSED(window);
  g_app->input.mouse_dx = x - g_app->input.mouse_x;
  g_app->input.mouse_dy = y - g_app->input.mouse_y;
  g_app->input.mouse_x  = x;
  g_app->input.mouse_y  = y;
}

static void glfw_on_mouse_wheel(GLFWwindow* window,
                                double xoffset,
                                double yoffset)
{
  UNUSED(window); UNUSED(xoffset);
  g_app->input.scroll_dy += yoffset;
}

static void update_fps_counter(Glsb_App* app, double curr_sec) {
  static double prev_update_sec = 0;
  static double prev_real_sec = 0;
  static int frame_count;

  double elapsed_real_sec = curr_sec - prev_real_sec;
  double elapsed_update_sec = curr_sec - prev_update_sec;
  prev_real_sec = curr_sec;

  if (elapsed_update_sec > 0.25) {
    char tmp[128];
    double fps = (double)frame_count / elapsed_update_sec;
    sprintf(tmp, "glsb @ fps: %.2f", fps);
    glfwSetWindowTitle(app->window, tmp);
    frame_count = 0;
    prev_update_sec = curr_sec;
  } else {
    frame_count += 1;
  }

  app->elapsed_secs = elapsed_real_sec;
}

static M4f update_projection_matrix(int w, int h) {
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
  return m;
}

