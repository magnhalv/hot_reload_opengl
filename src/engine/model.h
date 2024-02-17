#pragma once

#include "array.h"
#include "mesh.h"
#include "platform/types.h"

struct Model {
  i32 id;
  Array<Mesh> meshes;
  Transform transform;
};
