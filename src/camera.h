#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

#include "la.h"
#include <stdbool.h>

typedef struct {
  V3f pos;
  float yaw, pitch; // deg
  bool need_update;
  M4f cached_view;
} camera;

camera camera_init(V3f at);
M4f camera_view(camera* c);
void camera_yaw(camera* c, float deg);
void camera_pitch(camera* c, float deg);
void camera_forward(camera* c, float d);
void camera_right(camera* c, float d);

#endif // INCLUDE_CAMERA_H
