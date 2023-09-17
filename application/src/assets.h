#ifndef HOT_RELOAD_OPENGL_ASSETS_H
#define HOT_RELOAD_OPENGL_ASSETS_H

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <platform/types.h>

const i32 MESH_MAX_VERTICES = 256;

struct Mesh {
    GLuint vao;
    GLuint vertices_vbo;
    GLuint normals_vbo;
    GLuint mvp_vbo; // temp remove
    GLuint light_vbo; // temp remove
    glm::vec3 vertices[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_vertices;
    glm::vec3 normals[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_normals;

};

#endif //HOT_RELOAD_OPENGL_ASSETS_H
