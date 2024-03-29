#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <platform/types.h>

#include "array.h"
#include "asset_import.h"
#include "logger.h"
#include "material.h"
#include "memory_arena.h"
#include "mesh.h"

auto import_model(const char* path, Model& model, MemoryArena& storage) -> void {
  const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);

  if (scene == nullptr || !scene->HasMeshes()) {
    log_error("Unable to load %s: %s\n", path, aiGetErrorString());
    exit(1);
  } else {
    printf("Mesh loaded.\n");
  }

  model.meshes.init(storage, scene->mNumMeshes);

  for (i32 m = 0; m < scene->mNumMeshes; m++) {
    const auto& ai_mesh = scene->mMeshes[m];
    auto& mesh = model.meshes[m];
    mesh.id = m + 1;
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
  }
  aiReleaseImport(scene);
}
