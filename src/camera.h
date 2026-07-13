#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

#include <stdbool.h>

#include "app.h"
#include "la.h"

typedef struct {
  V3f pos;
  float yaw, pitch; // deg
  bool need_update;
  M4f cached_view;
} Camera;

Camera camera_init(V3f at);
M4f camera_view(Camera* c);
void camera_update(Camera* c, const Glsb_Input* input, double elapsed_secs);
void camera_yaw(Camera* c, float deg);
void camera_pitch(Camera* c, float deg);
void camera_forward(Camera* c, float d);
void camera_right(Camera* c, float d);
void camera_up(Camera* c, float d);

#endif // INCLUDE_CAMERA_H
