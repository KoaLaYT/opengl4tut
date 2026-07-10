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

Quat quat_init(float rad, V3f axis) {
  float half_rad = rad / 2.0;
  float s = sin(half_rad);
  float q0 = cos(half_rad);
  float q1 = s * axis.x;
  float q2 = s * axis.y;
  float q3 = s * axis.z;
  Quat q = {.q0 = q0, .q1 = q1, .q2 = q2, .q3 = q3};
  return q;
}

Quat quat_normalize(Quat q) {
  float r2 = q.q0*q.q0 + q.q1*q.q1 + q.q2*q.q2 + q.q3*q.q3;
  if (fabs(r2 - 1.0) >= 0.0001) {
    r2 = sqrt(r2);
    q.q0 /= r2;
    q.q1 /= r2;
    q.q2 /= r2;
    q.q3 /= r2;
  }
  return q;
}

Quat quat_conjugate(Quat q) {
  return (Quat){.q0 = q.q0, .q1 = -q.q1, .q2 = -q.q2, .q3 = -q.q3};
}

Quat quat_mul(Quat a, Quat b) {
  Quat q = {0};
  q.q0 = b.q0*a.q0 - b.q1*a.q1 - b.q2*a.q2 - b.q3*a.q3;
  q.q1 = b.q0*a.q1 + b.q1*a.q0 - b.q2*a.q3 + b.q3*a.q2;
  q.q2 = b.q0*a.q2 + b.q1*a.q3 + b.q2*a.q0 - b.q3*a.q1;
  q.q3 = b.q0*a.q3 - b.q1*a.q2 + b.q2*a.q1 + b.q3*a.q0;
  return q;
}

M4f quat_to_m4f(Quat q) {
  q = quat_normalize(q);

  M4f m = {0};
  float w = q.q0;
  float x = q.q1;
  float y = q.q2;
  float z = q.q3;

  m._11 = 1 - 2*y*y - 2*z*z;
  m._12 = 2*x*y + 2*w*z;
  m._13 = 2*x*z - 2*w*y;
  m._14 = 0;

  m._21 = 2*x*y - 2*w*z;
  m._22 = 1 - 2*x*x - 2*z*z;
  m._23 = 2*y*z + 2*w*x;
  m._24 = 0;

  m._31 = 2*x*z + 2*w*y;
  m._32 = 2*y*z - 2*w*x;
  m._33 = 1 - 2*x*x - 2*y*y;
  m._34 = 0;

  m._41 = 0;
  m._42 = 0;
  m._43 = 0;
  m._44 = 1;

  return m;
}
