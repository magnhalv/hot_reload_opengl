#ifndef HOT_RELOAD_OPENGL_ASSETS_H
#define HOT_RELOAD_OPENGL_ASSETS_H

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "types.h"

const i32 MESH_MAX_VERTICES = 256;

struct Mesh {
    GLuint vao;
    GLuint position_vbo;
    GLuint normals_vbo;
    glm::vec3 positions[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_positions;
    glm::vec3 normals[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_normals;

};

#endif //HOT_RELOAD_OPENGL_ASSETS_H
