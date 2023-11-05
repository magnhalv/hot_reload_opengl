#pragma once

#include <platform/types.h>
#include <math/vec3.h>
#include <math/transform.h>
#include "gl_shader.h"

const i32 MESH_MAX_VERTICES = 10000;

struct Mesh {
    GLVao vao;
    vec3 vertices[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_vertices;
    vec3 normals[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_normals;
    Transform transform;

};