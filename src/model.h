#ifndef INCLUDE_MODEL_H
#define INCLUDE_MODEL_H

#include <stddef.h>

#include <glad/gl.h>
#include <assimp/types.h>

#include "la.h"
#include "shader.h"

#define MAX_TEXTURE_LEN 32

typedef struct {
  V3f position;
  V3f normal;
  V2f tex_coords;
} Vertex;

typedef enum {
  TextureDiffuse,
  TextureSpecular,
} TextureType;

typedef struct {
  GLuint id;
  TextureType type;
  char path[AI_MAXLEN];
} Texture;

typedef struct {
  Vertex* vertices;
  size_t vertices_len;

  GLuint* indices;
  size_t indices_len;

  Texture* textures;
  size_t textures_len;

  GLuint VAO, VBO, EBO;
} Mesh;

typedef struct {
  Mesh* meshes;
  size_t meshes_len;
  char* directory;

  Texture loaded[MAX_TEXTURE_LEN];
  size_t loaded_len;
} Model;

Texture texture_init(const char* file_path, TextureType type);

Mesh mesh_init(Vertex*  vertices, size_t vertices_len,
               GLuint*  indices,  size_t indices_len,
               Texture* textures, size_t textures_len);
void mesh_deinit(Mesh* m);
void mesh_draw(const Mesh* m, Shader shader);

Model model_init(const char* path);
void model_deinit(Model* m);
void model_draw(const Model* m, Shader s);

#endif // INCLUDE_MODEL_H
