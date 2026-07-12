#ifndef INCLUDE_LA_H
#define INCLUDE_LA_H

#define PI 3.14159f
#define DEG_TO_RAD(deg) (deg*PI/180.0f)

// column major
typedef union {
  struct {
    float x,y;
  };
  float a[2];
} V2f;

typedef union {
  struct {
    float x,y,z;
  };
  float a[3];
} V3f;

typedef union {
  struct {
    float _11, _12, _13;
    float _21, _22, _23;
    float _31, _32, _33;
  };
  float a[9];
} M3f;

typedef union {
  struct {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
  };
  float a[16];
} M4f;

typedef union {
  struct {
    float q0,q1,q2,q3;
  };
  float a[4];
} Quat;

V2f v2f(float x, float y);

V3f v3f(float x, float y, float z);
V3f v3f_neg(V3f v);

M3f m3f_id();
V3f m3f_mul_vec(M3f m, V3f v);
M3f m3f_rot_y(float rad);

M4f m4f_id();
M4f m4f_mul(M4f a, M4f b);
M4f m4f_translate(M4f m, V3f v);
M4f m4f_scale(M4f m, float s);
M4f m4f_rot_x(float rad);
M4f m4f_rot_y(float rad);
M4f m4f_rot_z(float rad);

Quat quat_init(float rad, V3f axis);
Quat quat_normalize(Quat q);
Quat quat_conjugate(Quat q);
Quat quat_mul(Quat a, Quat b);
M4f quat_to_m4f(Quat q);

#endif // INCLUDE_LA_H
