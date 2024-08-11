#pragma once

#include <platform/types.h>

#include <math/math.h>
#include <math/transform.h>
#include <math/vec3.h>

#include "gl/gl_vao.h"
#include "material.h"

const i32 MESH_MAX_VERTICES = 10000;

struct BBox {
  f32 max_x;
  f32 min_x;
  f32 max_y;
  f32 min_y;
  f32 max_z;
  f32 min_z;
};

// TODO: Might want to have some pointer to some draw buffer when we mutate the mesh (e.g. for animation). So the mesh
// raw data will be considered read-only, but we have some buffer we copy it into, and do mutation there.
struct Mesh {
  i32 id{};
  GLVao vao{};
  vec3* vertices;
  i32 num_vertices{};
  vec3* normals;
  i32 num_normals{};
  Transform transform;
  Material material;

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
      result.min_x = hm::min(x, result.min_x);
      result.max_x = hm::max(x, result.max_x);

      f32 y = vertices[i].y;
      result.min_y = hm::min(y, result.min_y);
      result.max_y = hm::max(y, result.max_y);

      f32 z = vertices[i].z;
      result.min_z = hm::min(z, result.min_z);
      result.max_z = hm::max(z, result.max_z);
    }
    return result;
  }
};
