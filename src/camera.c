#include <math.h>

#include "camera.h"

#define MOUSE_SENSITIVITY 0.05
#define MOVE_SPEED        6

Camera camera_init(V3f at) {
  Camera c = {0};
  c.pos = at;
  c.need_update = true;
  return c;
}

M4f camera_view(Camera* c) {
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

void camera_update(Camera* c, const Glsb_Input* input, double elapsed_secs) {
  static bool first_mouse = true;
  static float last_xpos = 0;
  static float last_ypos = 0;

  if (input->keys[GLFW_KEY_W]) {
    camera_forward(c, +MOVE_SPEED*elapsed_secs);
  }
  if (input->keys[GLFW_KEY_S]) {
    camera_forward(c, -MOVE_SPEED*elapsed_secs);
  }
  if (input->keys[GLFW_KEY_A]) {
    camera_right(c, -MOVE_SPEED*elapsed_secs);
  }
  if (input->keys[GLFW_KEY_D]) {
    camera_right(c, +MOVE_SPEED*elapsed_secs);
  }
  if (input->keys[GLFW_KEY_UP]) {
    camera_up(c, +MOVE_SPEED*elapsed_secs);
  }
  if (input->keys[GLFW_KEY_DOWN]) {
    camera_up(c, -MOVE_SPEED*elapsed_secs);
  }

  if (input->mouse_buttons[GLFW_MOUSE_BUTTON_RIGHT]) {
    double xpos = input->mouse_x;
    double ypos = input->mouse_y;
    if (first_mouse) {
      last_xpos = xpos;
      last_ypos = ypos;
      first_mouse = false;
    }
    double dx = xpos - last_xpos;
    double dy = ypos - last_ypos;
    last_xpos = xpos;
    last_ypos = ypos;
    camera_yaw(c, -dx*MOUSE_SENSITIVITY);
    camera_pitch(c, -dy*MOUSE_SENSITIVITY);
  } else {
    first_mouse = true;
  }
}

void camera_yaw(Camera* c, float deg) {
  if (deg == 0) return;
  c->need_update = true;
  c->yaw += deg;
}

void camera_pitch(Camera* c, float deg) {
  if (deg == 0) return;
  c->need_update = true;
  c->pitch += deg;
  if (c->pitch >  89) c->pitch =  89;
  if (c->pitch < -89) c->pitch = -89;
}

void camera_forward(Camera* c, float d) {
  c->need_update = true;
  float yaw = DEG_TO_RAD(c->yaw);
  float x = d * sin(yaw);
  float z = d * cos(yaw);
  c->pos.x -= x;
  c->pos.z -= z;
}

void camera_right(Camera* c, float d) {
  c->need_update = true;
  float yaw = DEG_TO_RAD(c->yaw);
  float x = d * cos(yaw);
  float z = d * sin(yaw);
  c->pos.x += x;
  c->pos.z -= z;
}

void camera_up(Camera* c, float d) {
  c->need_update = true;
  c->pos.y += d;
}
