#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <stb_image.h>

#include "model.h"
#include "shader.h"
#include "tut.h"

static void mesh_setup(Mesh* m);
static char* find_directory(const char* path);
static int  count_meshes(const struct aiNode* node, int count);
static int  model_process_node(Model* m, struct aiNode* node, const struct aiScene* scene, int index);
static Mesh model_process_mesh(Model* m, struct aiMesh* mesh, const struct aiScene* scene);
static Texture model_load_texture(Model* m,
                                  struct aiMaterial* material,
                                  enum aiTextureType ai_type,
                                  TextureType type,
                                  unsigned int i,
                                  bool* loaded);

Mesh mesh_init(Vertex*  vertices, size_t vertices_len,
               GLuint*  indices,  size_t indices_len,
               Texture* textures, size_t textures_len)
{
  Mesh m = {0};
  m.vertices = vertices;
  m.vertices_len = vertices_len;
  m.indices = indices;
  m.indices_len = indices_len;
  m.textures = textures;
  m.textures_len = textures_len;
  mesh_setup(&m);
  return m;
}

void mesh_deinit(Mesh* m) {
  if (!m) return;
  if (m->vertices) {
    free(m->vertices);
    m->vertices = NULL;
    m->vertices_len = 0;
  }
  if (m->indices) {
    free(m->indices);
    m->indices = NULL;
    m->indices_len = 0;
  }
  if (m->textures) {
    free(m->textures);
    m->textures = NULL;
    m->textures_len = 0;
  }
}

void mesh_draw(const Mesh* m, Shader s) {
  char buf[128];
  int diffuse_nr  = 1;
  int specular_nr = 1;
  for (size_t i = 0; i < m->textures_len; i++) {
    glActiveTexture(GL_TEXTURE0+i);
    Texture tex = m->textures[i];
    switch (tex.type) {
      case TextureDiffuse:
        snprintf(buf, sizeof(buf), "material.texture_diffuse%d", diffuse_nr++);
        break;
      case TextureSpecular:
        snprintf(buf, sizeof(buf), "material.texture_specular%d", specular_nr++);
        break;
    }
    shader_set_int(s, buf, i);
    glBindTexture(GL_TEXTURE_2D, tex.id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(m->VAO);
  glDrawElements(GL_TRIANGLES, m->indices_len, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

Model model_init(const char* path) {
  Model m = {0};

  const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    const char* err = aiGetErrorString();
    tut_log_err("assimp aiImportFile: %s\n", err);
    exit(1);
  }

  m.directory = find_directory(path);
  m.meshes_len = count_meshes(scene->mRootNode, 0);
  m.meshes = malloc(sizeof(Mesh) * m.meshes_len);
  model_process_node(&m, scene->mRootNode, scene, 0);
  aiReleaseImport(scene);

  return m;
}

void model_deinit(Model* m) {
  if (!m) return;
  if (m->meshes) {
    for (size_t i = 0; i < m->meshes_len; i++) {
      mesh_deinit(m->meshes + i);
    }
    free(m->meshes);
    m->meshes = NULL;
    m->meshes_len = 0;
  }
  if (m->directory) {
    free(m->directory);
    m->directory = NULL;
  }
}

void model_draw(const Model* m, Shader s) {
  for (size_t i = 0; i < m->meshes_len; i++) {
    mesh_draw(m->meshes + i, s);
  }
}

Texture texture_init(const char* file_path, TextureType type) {
  stbi_set_flip_vertically_on_load(1);

  int w, h, n;
  int force_channels = 4;
  unsigned char* image_data = stbi_load(file_path, &w, &h, &n, force_channels);
  if (!image_data) {
    const char* err = stbi_failure_reason();
    tut_log_err("stbi_load %s failed: %s\n", file_path, err);
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

  return (Texture){.id = id, .type = type};
}

static void mesh_setup(Mesh* m) {
  glGenVertexArrays(1, &m->VAO);
  glGenBuffers(1, &m->VBO);
  glGenBuffers(1, &m->EBO);

  glBindVertexArray(m->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
  glBufferData(GL_ARRAY_BUFFER, m->vertices_len * sizeof(Vertex), m->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->indices_len * sizeof(GLuint), m->indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

  glBindVertexArray(0);
}

static int count_meshes(const struct aiNode* node, int count) {
  count += node->mNumMeshes;
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    count = count_meshes(node->mChildren[i], count);
  }
  return count;
}

static int model_process_node(Model* m, struct aiNode* node, const struct aiScene* scene, int index) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    m->meshes[index++] = model_process_mesh(m, mesh, scene);
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    index = model_process_node(m, node->mChildren[i], scene, index);
  }
  return index;
}

static Mesh model_process_mesh(Model* m, struct aiMesh* mesh, const struct aiScene* scene) {
  // process vertices
  size_t  vertices_len = mesh->mNumVertices;
  Vertex* vertices = malloc(vertices_len * sizeof(Vertex));
  for (unsigned int i = 0; i < vertices_len; i++) {
    Vertex vertex = {0};
    vertex.position = v3f(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z);
    vertex.normal = v3f(mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z);
    if (mesh->mTextureCoords[0]) {
      vertex.tex_coords = v2f(mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y);
    }
    vertices[i] = vertex;
  }

  // process indices
  size_t indices_len = 0;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    struct aiFace face = mesh->mFaces[i];
    indices_len += face.mNumIndices;
  }
  GLuint* indices = malloc(indices_len * sizeof(GLuint));
  size_t indices_count = 0;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    struct aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices[indices_count++] = face.mIndices[j];
    }
  }

  // process material
  size_t textures_len = 0;
  Texture* textures = NULL;
  struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  if (material) {
    size_t diffuse_len = aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE);
    size_t specular_len = aiGetMaterialTextureCount(material, aiTextureType_SPECULAR);
    textures_len = diffuse_len + specular_len;
    if (textures_len > 0) {
      textures = malloc(textures_len * sizeof(Texture));
      size_t textures_count = 0;

      for (unsigned int i = 0; i < diffuse_len; i++) {
        bool loaded = false;
        Texture tex = model_load_texture(m, material, aiTextureType_DIFFUSE, TextureDiffuse, i, &loaded);
        textures[textures_count++] = tex;
        if (loaded) {
          m->loaded[m->loaded_len++] = tex;
          assert(m->loaded_len <= MAX_TEXTURE_LEN);
        }
      }
      for (unsigned int i = 0; i < specular_len; i++) {
        bool loaded = false;
        Texture tex = model_load_texture(m, material, aiTextureType_SPECULAR, TextureSpecular, i, &loaded);
        textures[textures_count++] = tex;
        if (loaded) {
          m->loaded[m->loaded_len++] = tex;
          assert(m->loaded_len <= MAX_TEXTURE_LEN);
        }
      }
    }
  }

  return mesh_init(vertices, vertices_len,
                   indices,  indices_len,
                   textures, textures_len);
}

static Texture model_load_texture(Model* m,
                                  struct aiMaterial* material,
                                  enum aiTextureType ai_type,
                                  TextureType type,
                                  unsigned int i,
                                  bool* loaded)
{
  struct aiString path = {0};
  aiGetMaterialTexture(material, ai_type, i, &path,
      NULL, NULL, NULL, NULL, NULL, NULL);
  path.data[path.length] = 0;
  const char* cpath = path.data;

  for (unsigned int j = 0; j < m->loaded_len; j++) {
    if (strcmp(m->loaded[j].path, cpath) == 0) {
      *loaded = false;
      return m->loaded[j];
    }
  }

  char buf[AI_MAXLEN*2];
  snprintf(buf, sizeof(buf), "%s/%s", m->directory, cpath);
  Texture tex = texture_init(buf, type);
  snprintf(tex.path, sizeof(tex.path), "%s", cpath);
  *loaded = true;
  return tex;
}

static char* find_directory(const char* path) {
  size_t path_len = strlen(path);
  char* directory = malloc(path_len+1);
  strcpy(directory, path);
  int i = path_len - 1;
  for (; i >= 0; i--) {
    if (directory[i] == '/') {
      directory[i] = '\0';
      break;
    }
  }
  assert(i >= 0);
  return directory;
}
