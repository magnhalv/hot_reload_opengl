#ifndef HOT_RELOAD_OPENGL_MATERIAL_H
#define HOT_RELOAD_OPENGL_MATERIAL_H

#include <platform/types.h>
#include <math/vec4.h>
#include <math/vec3.h>

struct LightData
{
    vec4 omni_pos;
    vec4 omni_color;
    vec4 eye_pos;
};

struct Light {
    vec3 position_ws; // world space
    f32 radius{};
    vec3 color;

    [[nodiscard]] auto to_data(mat4 to_model_space, vec3 eye_pos) const -> LightData {
        vec4 pos_ms = to_model_space * to_vec4(position_ws);
        return {
            .omni_pos = vec4(pos_ms.x, pos_ms.y, pos_ms.z, 1/radius),
            .omni_color = to_vec4(color, 0.0f),
            .eye_pos = to_vec4(eye_pos, 0.0f)
        };
    }
};

struct Material {
    vec4 diffuse_color;
    vec4 specular_color;
    vec4 emission_color;
    vec4 ambient_color;
    float specular_exponent{};
};

inline auto get_material(const char *material_name) -> Material {
    if (std::strcmp(material_name, "metal") == 0) {
        return {
                .diffuse_color = vec4(0.3, 0.3, 0.3, 0),
                .specular_color = vec4(0.7, 0.7, 0.7, 0),
                .emission_color = vec4(0, 0, 0, 0),
                .ambient_color = vec4(0, 0, 0, 0),
                .specular_exponent = 70.0
        };
    }
    if (std::strcmp(material_name, "wood") == 0) {
        return {
                .diffuse_color = vec4(0.21, 0.15, 0.12, 0),
                .specular_color = vec4(0.1, 0.1, 0.1, 0),
                .emission_color = vec4(0, 0, 0, 0),
                .ambient_color = vec4(0.21 / 10.0, 0.15 / 10.0, 0.12 / 10.0, 0),
                .specular_exponent = 20
        };
    }
    if (std::strcmp(material_name, "wall_dark") == 0) {
        return {
                .diffuse_color = vec4(0.4, 0.4, 0.4, 0),
                .specular_color = vec4(0.0, 0.0, 0.0, 0),
                .emission_color = vec4(0, 0, 0, 0),
                .ambient_color = vec4(0.4 / 10.0, 0.4 / 10.0, 0.4 / 10.0, 0),
                .specular_exponent = 0
        };
    }
    if (std::strcmp(material_name, "wall_light") == 0) {
        return {
                .diffuse_color = vec4(0.8, 0.8, 0.8, 0),
                .specular_color = vec4(0.0, 0.0, 0.0, 0),
                .emission_color = vec4(0, 0, 0, 0),
                .ambient_color = vec4(0.7 / 10.0, 0.7 / 10.0, 0.7 / 10.0, 0),
                .specular_exponent = 0
        };
    }
    printf("WARNING: Unknown material: %s\n", material_name);
    return {
            .diffuse_color = vec4(205 / 255.0, 117 / 255.0, 132 / 255.0, 0),
            .emission_color = vec4(0, 0, 0, 0),
            .ambient_color = vec4(205 / 255.0, 117 / 255.0, 132 / 255.0, 0),
            .specular_exponent = 1000
    };
}


#endif //HOT_RELOAD_OPENGL_MATERIAL_H
