#ifndef INCLUDE_LA_H
#define INCLUDE_LA_H

// column major
typedef union {
  struct {
    float x,y,z;
  };
  float a[3];
} V3f;

typedef union {
  struct {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
  };
  float a[16];
} M4f;

V3f v3f(float x, float y, float z);
V3f v3f_neg(V3f v);

M4f m4f_id();
M4f m4f_mul(M4f a, M4f b);
M4f m4f_translate(V3f v);
M4f m4f_rot_y(float rad);
M4f m4f_rot_z(float rad);

#endif // INCLUDE_LA_H
