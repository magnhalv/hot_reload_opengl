#ifndef HOT_RELOAD_OPENGL_ASSETS_H
#define HOT_RELOAD_OPENGL_ASSETS_H

#include <glad/gl.h>

#include <platform/types.h>
#include <math/vec3.h>
#include <math/transform.h>

const i32 MESH_MAX_VERTICES = 256;

struct Mesh {
    GLuint vao;
    GLuint vertices_vbo;
    GLuint normals_vbo;
    GLuint mvp_vbo; // temp remove
    GLuint light_vbo; // temp remove
    vec3 vertices[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_vertices;
    vec3 normals[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_normals;
    Transform transform;

};

#endif //HOT_RELOAD_OPENGL_ASSETS_H
