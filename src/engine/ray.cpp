#include <cmath>

#include "mesh.h"
#include "ray.h"

// Calculates when a ray from origin, with a given direction, will hit axis_min and axis_max
// i.e. how much you must multiply direction with to get axis_min and axis_max from origin.
vec2 intersections_on_axis(f32 origin, f32 direction, f32 axis_min, f32 axis_max) {
  f32 tmin_numerator = axis_min - origin;
  f32 tmax_numerator = axis_max - origin;

  f32 tmin;
  f32 tmax;

  f32 EPSILON = 0.0001f;
  if (abs(origin) >= EPSILON || origin == 0) {
    tmin = tmin_numerator / direction;
    tmax = tmax_numerator / direction;
  } else {
    tmin = tmin_numerator * F32_MAX;
    tmax = tmax_numerator * F32_MAX;
  }

  if (tmin > tmax) {
    f32 temp = tmax;
    tmax = tmin;
    tmin = temp;
  }
  return vec2(tmin, tmax);
}

bool intersects(vec3 from_pos, vec3 ray, BBox bbox, Transform& transform, vec2& intersections) {
  vec3 from_pos_local = to_vec3(inverse(transform.to_mat4()) * to_vec4(from_pos)); // move from_pos to mesh coord-space

  Transform origin_transform;
  origin_transform.rotation = transform.rotation;
  vec3 ray_local = to_vec3(inverse(origin_transform.to_mat4()) * to_vec4(ray));

  return intersects(ray_local, from_pos_local, bbox, intersections);
}

bool intersects(const vec3& ray, const vec3& ray_origin, const BBox& bbox, vec2& intersections) {
  vec2 x = intersections_on_axis(ray_origin.x, ray.x, bbox.min_x, bbox.max_x);
  vec2 y = intersections_on_axis(ray_origin.y, ray.y, bbox.min_y, bbox.max_y);
  vec2 z = intersections_on_axis(ray_origin.z, ray.z, bbox.min_z, bbox.max_z);

  f32 tmin = hm::max(hm::max(x.v[0], y.v[0]), z.v[0]);
  f32 tmax = hm::min(hm::min(x.v[1], y.v[1]), z.v[1]);

  if (std::isinf(tmin) || std::isinf(tmax)) {
    return false;
  }

  if (tmin > tmax) {
    return false;
  }

  intersections.x = tmin;
  intersections.y = tmax;
  return true;
}

bool intersects(const vec3& ray, const vec3& ray_origin, const BBox& bbox) {
  vec2 x = intersections_on_axis(ray_origin.x, ray.x, bbox.min_x, bbox.max_x);
  vec2 y = intersections_on_axis(ray_origin.y, ray.y, bbox.min_y, bbox.max_y);
  vec2 z = intersections_on_axis(ray_origin.z, ray.z, bbox.min_z, bbox.max_z);

  f32 tmin = hm::max(hm::max(x.v[0], y.v[0]), z.v[0]);
  f32 tmax = hm::min(hm::min(x.v[1], y.v[1]), z.v[1]);

  return tmin <= tmax;
}
