#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <platform/types.h>

#include "array.h"
#include "asset_import.h"
#include "logger.h"
#include "material.h"
#include "math/transform.h"
#include "memory_arena.h"
#include "mesh.h"

static inline glm::mat4 convert_to_glm_format(const aiMatrix4x4& from) {
  glm::mat4 to;
  // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
  to[0][0] = from.a1;
  to[1][0] = from.a2;
  to[2][0] = from.a3;
  to[3][0] = from.a4;
  to[0][1] = from.b1;
  to[1][1] = from.b2;
  to[2][1] = from.b3;
  to[3][1] = from.b4;
  to[0][2] = from.c1;
  to[1][2] = from.c2;
  to[2][2] = from.c3;
  to[3][2] = from.c4;
  to[0][3] = from.d1;
  to[1][3] = from.d2;
  to[2][3] = from.d3;
  to[3][3] = from.d4;
  return to;
}

auto import_model(const char* path, Model& model, MemoryArena& storage) -> void {
  const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);

  if (scene == nullptr || !scene->HasMeshes()) {
    log_error("Unable to load %s: %s\n", path, aiGetErrorString());
    exit(1);
  } else {
    printf("Scene loaded.\n");
  }

  model.transform = Transform();
  model.meshes.init(storage, scene->mNumMeshes);

  for (i32 m = 0; m < scene->mNumMeshes; m++) {
    const auto& ai_mesh = scene->mMeshes[m];
    auto& mesh = model.meshes[m];
    mesh.id = m;
    if (ai_mesh->mNumVertices > MESH_MAX_VERTICES) {
      log_warning("Loaded mesh with more than 5000 vertices: %s", path);
    }

    auto material_name = scene->mMaterials[ai_mesh->mMaterialIndex]->GetName();
    mesh.material = get_material(material_name.C_Str());

    // TODO: Store indices, not duplicates of verticies and normals
    auto total_verticies = ai_mesh->mNumFaces * 3;
    mesh.vertices = allocate<vec3>(storage, total_verticies);
    mesh.normals = allocate<vec3>(storage, total_verticies);

    for (u32 f = 0; f < ai_mesh->mNumFaces; f++) {
      const auto& ai_face = ai_mesh->mFaces[f];
      assert(ai_face.mNumIndices == 3);
      const u32 indices[3] = { ai_face.mIndices[0], ai_face.mIndices[1], ai_face.mIndices[2] };
      for (const auto idx : indices) {
        assert(idx < ai_mesh->mNumVertices);
        const auto& ai_vec = ai_mesh->mVertices[idx];
        mesh.vertices[mesh.num_vertices++] = { ai_vec.x, ai_vec.z, -ai_vec.y };

        const auto& ai_norm = ai_mesh->mNormals[idx];
        mesh.normals[mesh.num_normals++] = { ai_norm.x, ai_norm.z, -ai_norm.y };
      }
      ASSERT_LEQ(mesh.num_normals, total_verticies);
      ASSERT_LEQ(mesh.num_vertices, total_verticies);
    }
    ASSERT_EQ(mesh.num_normals, total_verticies);
    ASSERT_EQ(mesh.num_vertices, total_verticies);
    mesh.transform = Transform();
  }
  aiReleaseImport(scene);
}
