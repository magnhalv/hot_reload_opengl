#include "ray.h"

// Calculates when a ray from origin, with a given direction, will hit axis_min and axis_max
// i.e. how much you must multiply direction with to get axis_min and axis_max from origin.
vec2 intersections_on_axis(f32 origin, f32 direction, f32 axis_min, f32 axis_max)
{
    f32 tmin_numerator = axis_min - origin;
    f32 tmax_numerator = axis_max - origin;

    f32 tmin;
    f32 tmax;

    f32 EPSILON = 0.0001f;
    if (abs(origin) >= EPSILON)
    {
        tmin = tmin_numerator / direction;
        tmax = tmax_numerator / direction;
    }
    else
    {
        tmin = tmin_numerator * F32_MAX;
        tmax = tmax_numerator * F32_MAX;
    }

    if (tmin > tmax)
    {
        f32 temp = tmax;
        tmax = tmin;
        tmin = temp;
    }
    return vec2(tmin, tmax);
}

bool intersects(vec3 from_pos, vec3 ray, Mesh &mesh, vec2 &intersections) {
    auto bbox = mesh.get_bbox();
    const mat4 mesh_trans = mesh.transform.to_mat4();
    vec3 from_pos_local = to_vec3(inverse(mesh_trans) * to_vec4(from_pos)); // move from_pos to mesh coord-space

    Transform origin_transform;
    origin_transform.rotation = mesh.transform.rotation;
    vec3 ray_local = to_vec3(inverse(origin_transform.to_mat4()) * to_vec4(ray));

    return intersects(ray_local, from_pos_local, mesh.get_bbox(), intersections);
}

bool intersects(const vec3 &ray, const vec3 &ray_origin, const BBox &bbox, vec2 &intersections)
{
    vec2 x = intersections_on_axis(ray_origin.x, ray.x, bbox.min_x, bbox.max_x);
    vec2 y = intersections_on_axis(ray_origin.y, ray.y, bbox.min_y, bbox.max_y);
    vec2 z = intersections_on_axis(ray_origin.z, ray.z, bbox.min_z, bbox.max_z);

    f32 tmin = max(max(x.v[0], y.v[0]), z.v[0]);
    f32 tmax = min(min(x.v[1], y.v[1]), z.v[1]);

    if (std::isinf(tmin) || std::isinf(tmax)) {
        return false;
    }

    if (tmin > tmax || tmin < 0)
    {
        return false;
    }

    intersections.x = tmin;
    intersections.y = tmax;
    return true;
}

bool intersects(const vec3 &ray, const vec3 &ray_origin, const BBox &bbox)
{
    vec2 x = intersections_on_axis(ray_origin.x, ray.x, bbox.min_x, bbox.max_x);
    vec2 y = intersections_on_axis(ray_origin.y, ray.y, bbox.min_y, bbox.max_y);
    vec2 z = intersections_on_axis(ray_origin.z, ray.z, bbox.min_z, bbox.max_z);

    f32 tmin = max(max(x.v[0], y.v[0]), z.v[0]);
    f32 tmax = min(min(x.v[1], y.v[1]), z.v[1]);

    return tmin <= tmax;
}