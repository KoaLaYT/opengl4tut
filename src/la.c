#include "la.h"
#include <math.h>

V3f v3f(float x, float y, float z) {
  V3f v = {0};
  v.x = x;
  v.y = y;
  v.z = z;
  return v;
}

V3f v3f_neg(V3f v) {
  V3f vv = {.x = -v.x, .y = -v.y, .z = -v.z};
  return vv;
}

M4f m4f_id() {
  M4f m = {0};
  m._11 = 1.0f;
  m._22 = 1.0f;
  m._33 = 1.0f;
  m._44 = 1.0f;
  return m;
}

M4f m4f_mul(M4f a, M4f b) {
  M4f m = {0};

  m._11 = a._11*b._11 + a._21*b._12 + a._31*b._13 + a._41*b._14;
  m._12 = a._12*b._11 + a._22*b._12 + a._32*b._13 + a._42*b._14;
  m._13 = a._13*b._11 + a._23*b._12 + a._33*b._13 + a._43*b._14;
  m._14 = a._14*b._11 + a._24*b._12 + a._34*b._13 + a._44*b._14;

  m._21 = a._11*b._21 + a._21*b._22 + a._31*b._23 + a._41*b._24;
  m._22 = a._12*b._21 + a._22*b._22 + a._32*b._23 + a._42*b._24;
  m._23 = a._13*b._21 + a._23*b._22 + a._33*b._23 + a._43*b._24;
  m._24 = a._14*b._21 + a._24*b._22 + a._34*b._23 + a._44*b._24;

  m._31 = a._11*b._31 + a._21*b._32 + a._31*b._33 + a._41*b._34;
  m._32 = a._12*b._31 + a._22*b._32 + a._32*b._33 + a._42*b._34;
  m._33 = a._13*b._31 + a._23*b._32 + a._33*b._33 + a._43*b._34;
  m._34 = a._14*b._31 + a._24*b._32 + a._34*b._33 + a._44*b._34;

  m._41 = a._11*b._41 + a._21*b._42 + a._31*b._43 + a._41*b._44;
  m._42 = a._12*b._41 + a._22*b._42 + a._32*b._43 + a._42*b._44;
  m._43 = a._13*b._41 + a._23*b._42 + a._33*b._43 + a._43*b._44;
  m._44 = a._14*b._41 + a._24*b._42 + a._34*b._43 + a._44*b._44;

  return m;
}

M4f m4f_translate(V3f v) {
  M4f m = m4f_id();
  m._41 = v.x;
  m._42 = v.y;
  m._43 = v.z;
  return m;
}

M4f m4f_rot_y(float rad) {
  M4f m = m4f_id();

  float s = sin(rad);
  float c = cos(rad);

  m._11 = c;
  m._13 = -s;
  m._31 = s;
  m._33 = c;

  return m;
}

M4f m4f_rot_z(float rad) {
  M4f m = m4f_id();

  float s = sin(rad);
  float c = cos(rad);

  m._11 = c;
  m._12 = s;
  m._21 = -s;
  m._22 = c;

  return m;
}
