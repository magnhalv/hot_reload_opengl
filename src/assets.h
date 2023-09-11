#ifndef HOT_RELOAD_OPENGL_ASSETS_H
#define HOT_RELOAD_OPENGL_ASSETS_H

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "types.h"

struct Mesh {
    GLuint vao;
    GLuint position_vbo;
    GLuint normals_vbo;
    glm::vec3 positions[256]; // TODO: Make this dynamic
    i32 num_positions;
    glm::vec3 normals[256]; // TODO: Make this dynamic
    i32 num_normals;

};

#endif //HOT_RELOAD_OPENGL_ASSETS_H
