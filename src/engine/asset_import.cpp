#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <platform/types.h>
#include <glm/glm.hpp>

#include "asset_import.h"
#include "logger.h"

static inline glm::mat4 convert_to_glm_format(const aiMatrix4x4& from)
{
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

void import_mesh(const char *path, Mesh* mesh) {
    const aiScene *scene = aiImportFile(path, aiProcess_Triangulate);

    if (scene == nullptr || !scene->HasMeshes()) {
        log_error("Unable to load %s: %s\n", path, aiGetErrorString());
        exit(1);
    } else {
        printf("Scene loaded.\n");
    }

    assert(scene->mNumMeshes == 1);
    for (i32 m = 0; m < scene->mNumMeshes; m++) {
        const auto &ai_mesh = scene->mMeshes[m];

        printf("%d %d", ai_mesh->mNumVertices, MESH_MAX_VERTICES);
        assert(ai_mesh->mNumVertices < MESH_MAX_VERTICES);

        for (u32 f = 0; f < ai_mesh->mNumFaces; f++) {
            const auto &ai_face = ai_mesh->mFaces[f];
            assert(ai_face.mNumIndices == 3);
            const u32 indices[3] = {ai_face.mIndices[0], ai_face.mIndices[1], ai_face.mIndices[2]};
            for (const auto idx: indices) {
                assert(idx < ai_mesh->mNumVertices);
                const auto &ai_vec = ai_mesh->mVertices[idx];
                mesh->vertices[mesh->num_vertices++] = {ai_vec.x, ai_vec.y, ai_vec.z};

                const auto &ai_norm = ai_mesh->mNormals[idx];
                mesh->normals[mesh->num_normals++] = {ai_norm.x, ai_norm.y, ai_norm.z};
            }
        }
        mesh->transform = Transform();
    }
    log_info("Converted from assimp to local representation.");
    log_info("Num vertices %d", mesh->num_vertices);
    aiReleaseImport(scene);
}

