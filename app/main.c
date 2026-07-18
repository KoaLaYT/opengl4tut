#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "tut.h"
#include "la.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

#define MOUSE_SENSITIVITY 0.05
#define MOVE_SPEED        6

static int g_win_width  = 640;
static int g_win_height = 480;
static M4f g_projection = {0};
static Camera g_camera = {0};
static bool g_mouse_button_right_pressed = false;
static bool g_first_mouse = true;
static float g_last_xpos = 0;
static float g_last_ypos = 0;

static double update_fps_counter(GLFWwindow* window);
static void glfw_error_callback(int error, const char* description);
static void glfw_framebuffer_size_callback(GLFWwindow* win, int width, int height);
static void update_projection_matrix(int width, int height);
static void process_keypress(GLFWwindow* win, float elapsed_secs);
static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main(void) {
  tut_restart_log();

  tut_log_info("starting GLFW %s\n", glfwGetVersionString());

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    tut_log_err("could not start GLFW3\n");
    return 1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // anti-aliasing

  GLFWwindow* window = glfwCreateWindow(g_win_width, g_win_height, "", NULL, NULL);
  if (!window) {
    tut_log_err("could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
  glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
  glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);

  gladLoadGL(glfwGetProcAddress);
  tut_log_glparams();

  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"
  // glEnable(GL_CULL_FACE);  // cull face
  // glCullFace(GL_BACK);     // cull back face
  // glFrontFace(GL_CW);      // clock-wise

  GLfloat vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint cube_vao = 0;
  glGenVertexArrays(1, &cube_vao);
  glBindVertexArray(cube_vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), NULL);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  GLuint light_vao = 0;
  glGenVertexArrays(1, &light_vao);
  glBindVertexArray(light_vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  V3f cube_positions[] = {
    v3f( 0.0f,  0.0f,  0.0f),
    v3f( 2.0f,  5.0f, -15.0f),
    v3f(-1.5f, -2.2f, -2.5f),
    v3f(-3.8f, -2.0f, -12.3f),
    v3f( 2.4f, -0.4f, -3.5f),
    v3f(-1.7f,  3.0f, -7.5f),
    v3f( 1.3f, -2.0f, -2.5f),
    v3f( 1.5f,  2.0f, -2.5f),
    v3f( 1.5f,  0.2f, -1.5f),
    v3f(-1.3f,  1.0f, -1.5f)
  };

  V3f point_light_positions[] = {
	  v3f( 0.7f,  0.2f,  2.0f),
	  v3f( 2.3f, -3.3f, -4.0f),
	  v3f(-4.0f,  2.0f, -12.0f),
	  v3f( 0.0f,  0.0f, -3.0f)
  };

  Texture diffuse_tex = texture_init("asset/container.png", TextureDiffuse);
  Texture spcular_tex = texture_init("asset/container_specular.png", TextureSpecular);
  Glsb_Shader cube_shader  = glsb_shader_init("glsl/cube.vert", "glsl/cube.frag");
  Glsb_Shader light_shader = glsb_shader_init("glsl/light.vert", "glsl/light.frag");

  g_camera = camera_init(v3f(0,0,10));
  V3f light_color = v3f(1.0,1.0,1.0);
  V3f diffuse_color = v3f(light_color.x*0.5,light_color.y*0.5,light_color.z*0.5);
  V3f ambient_color = v3f(light_color.x*0.1,light_color.y*0.1,light_color.z*0.1);
  glsb_shader_use(cube_shader);
  // directional light
  glsb_shader_set_v3f(cube_shader, "dir_light.ambient", ambient_color);
  glsb_shader_set_v3f(cube_shader, "dir_light.diffuse", diffuse_color);
  glsb_shader_set_v3f(cube_shader, "dir_light.specular", v3f(1.0,1.0,1.0));
  glsb_shader_set_v3f(cube_shader, "dir_light.direction", v3f(-0.2,-1.0,-0.3));
  // point light 0
  glsb_shader_set_v3f(cube_shader, "point_lights[0].ambient", ambient_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[0].diffuse", diffuse_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[0].specular", v3f(1.0,1.0,1.0));
  glsb_shader_set_v3f(cube_shader, "point_lights[0].position", point_light_positions[0]);
  glsb_shader_set_float(cube_shader, "point_lights[0].constant", 1.0);
  glsb_shader_set_float(cube_shader, "point_lights[0].linear", 0.09);
  glsb_shader_set_float(cube_shader, "point_lights[0].quadratic", 0.032);
  // point light 1
  glsb_shader_set_v3f(cube_shader, "point_lights[1].ambient", ambient_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[1].diffuse", diffuse_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[1].specular", v3f(1.0,1.0,1.0));
  glsb_shader_set_v3f(cube_shader, "point_lights[1].position", point_light_positions[1]);
  glsb_shader_set_float(cube_shader, "point_lights[1].constant", 1.0);
  glsb_shader_set_float(cube_shader, "point_lights[1].linear", 0.09);
  glsb_shader_set_float(cube_shader, "point_lights[1].quadratic", 0.032);
  // point light 2
  glsb_shader_set_v3f(cube_shader, "point_lights[2].ambient", ambient_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[2].diffuse", diffuse_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[2].specular", v3f(1.0,1.0,1.0));
  glsb_shader_set_v3f(cube_shader, "point_lights[2].position", point_light_positions[2]);
  glsb_shader_set_float(cube_shader, "point_lights[2].constant", 1.0);
  glsb_shader_set_float(cube_shader, "point_lights[2].linear", 0.09);
  glsb_shader_set_float(cube_shader, "point_lights[2].quadratic", 0.032);
  // point light 3
  glsb_shader_set_v3f(cube_shader, "point_lights[3].ambient", ambient_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[3].diffuse", diffuse_color);
  glsb_shader_set_v3f(cube_shader, "point_lights[3].specular", v3f(1.0,1.0,1.0));
  glsb_shader_set_v3f(cube_shader, "point_lights[3].position", point_light_positions[3]);
  glsb_shader_set_float(cube_shader, "point_lights[3].constant", 1.0);
  glsb_shader_set_float(cube_shader, "point_lights[3].linear", 0.09);
  glsb_shader_set_float(cube_shader, "point_lights[3].quadratic", 0.032);
  // flash light
  glsb_shader_set_v3f(cube_shader, "flash_light.ambient", ambient_color);
  glsb_shader_set_v3f(cube_shader, "flash_light.diffuse", diffuse_color);
  glsb_shader_set_v3f(cube_shader, "flash_light.specular", v3f(1.0,1.0,1.0));
  glsb_shader_set_float(cube_shader, "flash_light.inner_cutoff", cos(DEG_TO_RAD(12.5)));
  glsb_shader_set_float(cube_shader, "flash_light.outer_cutoff", cos(DEG_TO_RAD(17.5)));

  glsb_shader_set_int(cube_shader, "material.diffuse", 0);
  glsb_shader_set_int(cube_shader, "material.specular", 1);
  glsb_shader_set_float(cube_shader, "material.shininess", 32.0f);

  while (!glfwWindowShouldClose(window)) {
    double elapsed_secs = update_fps_counter(window);

    process_keypress(window, elapsed_secs);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    M4f view = camera_view(&g_camera);
    {
      glsb_shader_use(cube_shader);
      for (int i = 0; i < 10; i++) {
        M4f t = m4f_translate(cube_positions[i]);
        M4f rx = m4f_rot_x(DEG_TO_RAD(20.0*i));
        M4f ry = m4f_rot_y(DEG_TO_RAD(20.0*i));
        M4f rz = m4f_rot_z(DEG_TO_RAD(20.0*i));
        M4f model = m4f_mul(t, rx, ry, rz);
        glsb_shader_set_m4f(cube_shader, "model", model);
        glsb_shader_set_m4f(cube_shader, "view", view);
        glsb_shader_set_m4f(cube_shader, "projection", g_projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_tex.id);
        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, spcular_tex.id);
        glBindVertexArray(cube_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
    {
      glsb_shader_use(light_shader);
      for (int i = 0; i < 4; i++) {
        M4f t = m4f_translate(point_light_positions[i]);
        M4f s = m4f_scale(0.2);
        M4f model = m4f_mul(t, s);
        glsb_shader_set_v3f(light_shader, "light_color", light_color);
        glsb_shader_set_m4f(light_shader, "model", model);
        glsb_shader_set_m4f(light_shader, "view", view);
        glsb_shader_set_m4f(light_shader, "projection", g_projection);
        glBindVertexArray(light_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

static double update_fps_counter(GLFWwindow* window)
{
  static double prev_update_sec = 0;
  static double prev_real_sec = 0;
  static int frame_count;

  double curr_sec = glfwGetTime();
  double elapsed_real_sec = curr_sec - prev_real_sec;
  double elapsed_update_sec = curr_sec - prev_update_sec;
  prev_real_sec = curr_sec;

  if (elapsed_update_sec > 0.25) {
    char tmp[128];
    double fps = (double)frame_count / elapsed_update_sec;
    sprintf(tmp, "opengl @ fps: %.2f", fps);
    glfwSetWindowTitle(window, tmp);
    frame_count = 0;
    prev_update_sec = curr_sec;
  } else {
    frame_count += 1;
  }

  return elapsed_real_sec;
}

static void glfw_error_callback(int error, const char* description) {
  tut_log_err("GLFW code: %i, description: %s\n", error, description);
}

static void glfw_framebuffer_size_callback(GLFWwindow* win, int width, int height) {
  (void)win;
  glViewport(0, 0, width, height);
  update_projection_matrix(width, height);
}

static void update_projection_matrix(int w, int h) {
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
  g_projection = m;
}

static void process_keypress(GLFWwindow* window, float elapsed_secs) {
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
    camera_forward(&g_camera, +MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
    camera_forward(&g_camera, -MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
    camera_right(&g_camera, -MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
    camera_right(&g_camera, +MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) {
    camera_up(&g_camera, +MOVE_SPEED*elapsed_secs);
  }
  if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) {
    camera_up(&g_camera, -MOVE_SPEED*elapsed_secs);
  }
}

static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
  (void)window;

  if (g_mouse_button_right_pressed) {
    if (g_first_mouse) {
      g_last_xpos = xpos;
      g_last_ypos = ypos;
      g_first_mouse = false;
    }

    double dx = xpos - g_last_xpos;
    double dy = ypos - g_last_ypos;
    g_last_xpos = xpos;
    g_last_ypos = ypos;
    camera_yaw(&g_camera, -dx*MOUSE_SENSITIVITY);
    camera_pitch(&g_camera, -dy*MOUSE_SENSITIVITY);
  }
}

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  (void)window;
  (void)mods;

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    g_mouse_button_right_pressed = true;
  } else {
    g_mouse_button_right_pressed = false;
    g_first_mouse = true;
  }
}
