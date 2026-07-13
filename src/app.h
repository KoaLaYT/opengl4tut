#ifndef INCLUDE_APPLICATION_H
#define INCLUDE_APPLICATION_H

#include <stdbool.h>

#include <GLFW/glfw3.h>

#include "la.h"

typedef struct {
  int window_width;
  int window_height;
  int major_version;
  int minor_version;
  int samples;
  union {
    struct {
      unsigned int fullscreen: 1;
      unsigned int vsync     : 1;
      unsigned int cursor    : 1;
      unsigned int stereo    : 1;
      unsigned int debug     : 1;
      unsigned int robust    : 1;
    };
    unsigned int all;
  } flags;
} Glsb_AppInfo;

typedef struct {
  bool keys[GLFW_KEY_LAST+1];
  bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST+1];
  double mouse_x, mouse_y;
  double mouse_dx, mouse_dy;
  double scroll_dy;
} Glsb_Input;

typedef struct _Glsb_App Glsb_App;

typedef void Glsb_AppSetupFn(Glsb_App* app);
typedef void Glsb_AppStartupFn(Glsb_App* app);
typedef void Glsb_AppRenderFn(Glsb_App* app, double current_time);
typedef void Glsb_AppShutdownFn(Glsb_App* app);

struct _Glsb_App {
  GLFWwindow* window;
  Glsb_AppInfo info;
  Glsb_Input input;
  double elapsed_secs;
  M4f projection;
  void* context;

  Glsb_AppSetupFn*         setup;           // required
  Glsb_AppStartupFn*       startup;         // optional
  Glsb_AppRenderFn*        render;          // optional
  Glsb_AppShutdownFn*      shutdown;        // optional
};

Glsb_App* glsb_app_init();
void glsb_app_deinit();
void glsb_app_run();

void glsb_app_setup_default(Glsb_App* app);

#endif // INCLUDE_APPLICATION_H
