#include "camera.h"
#include <math.h>

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

    M4f t = m4f_translate(m4f_id(), v3f_neg(c->pos));
    M4f r = quat_to_m4f(q_inv);
    c->cached_view = m4f_mul(r, t);
    c->need_update = false;
  }
  return c->cached_view;
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
