#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "tut.h"

int main() {
  const char* model_path = "non_exist";
  const struct aiScene* scene = aiImportFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    const char* err = aiGetErrorString();
    tut_log_err("assimp import file: %s\n", err);
    return 1;
  }

  return 0;
}
