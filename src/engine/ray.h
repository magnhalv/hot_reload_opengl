#pragma once

#include <cmath>

#include <platform/types.h>

#include "math/vec2.h"
#include "math/vec3.h"
#include "mesh.h"


bool intersects(const vec3 &ray, const vec3 &ray_origin, const BBox &bbox, vec2 &intersections);
bool intersects(const vec3 &ray, const vec3 &ray_origin, const BBox &bbox);
bool intersects(vec3 from_pos, vec3 ray, Mesh &mesh, vec2 &intersections);
