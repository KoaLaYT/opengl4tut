#include <stb_image.h>
#include <stdlib.h>
#include "texture.h"
#include "tut.h"

Texture texture_init(const char* file_path) {
  stbi_set_flip_vertically_on_load(1);

  int w, h, n;
  int force_channels = 4;
  unsigned char* image_data = stbi_load(file_path, &w, &h, &n, force_channels);
  if (!image_data) {
    const char* err = stbi_failure_reason();
    tut_log_err("load %s failed: %s\n", file_path, err);
    exit(1);
  }

  if ((w & (w-1)) != 0 || (h & (h-1)) != 0) {
    tut_log_debug("texture %s is not power-of-2 dimensions\n", file_path);
  }

  GLuint id = 0;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  stbi_image_free(image_data);

  return (Texture){.id = id};
}
