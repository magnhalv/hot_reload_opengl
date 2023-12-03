#pragma once

#include <platform/types.h>

#include <math/vec3.h>
#include <math/math.h>
#include <math/transform.h>

#include "gl_shader.h"

const i32 MESH_MAX_VERTICES = 10000;

struct BBox {
    f32 max_x;
    f32 min_x;
    f32 max_y;
    f32 min_y;
    f32 max_z;
    f32 min_z;
};

struct Mesh {
    GLVao vao;
    vec3 vertices[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_vertices;
    vec3 normals[MESH_MAX_VERTICES]; // TODO: Make this dynamic
    i32 num_normals;
    Transform transform;

    auto get_bbox() -> BBox {
        BBox result = {};
        result.min_x = vertices[0].x;
        result.max_x = vertices[0].x;
        result.min_y = vertices[0].y;
        result.max_y = vertices[0].y;
        result.min_z = vertices[0].z;
        result.max_z = vertices[0].z;
        for (i32 i = 0; i < num_vertices; i++) {
            f32 x = vertices[i].x;
            result.min_x = min(x, result.min_x);
            result.max_x = max(x, result.max_x);

            f32 y = vertices[i].y;
            result.min_y = min(y, result.min_y);
            result.max_y = max(y, result.max_y);

            f32 z = vertices[i].z;
            result.min_z = min(z, result.min_z);
            result.max_z = max(z, result.max_z);
        }
        return result;
    }
};