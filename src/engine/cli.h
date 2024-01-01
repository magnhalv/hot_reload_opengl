#ifndef HOT_RELOAD_OPENGL_CLI_H
#define HOT_RELOAD_OPENGL_CLI_H

#include <platform/types.h>

#include "gl/gl.h"

inline auto to_ndc_tmp(u32 pixels, u32 range) -> f32 {
    auto x = range / 2;
    return (static_cast<f32>(pixels) / static_cast<f32>(x)) - 1;
}



void cli_draw(u32 width, u32 height) {
    u32 min_height = height/2;
    f32 cursor_vertices[] = {
            to_ndc_tmp(0, width), to_ndc_tmp(min_height, height),
            to_ndc_tmp(0, width), to_ndc_tmp(height, height),
            to_ndc_tmp(width, width), to_ndc_tmp(height, height),

            to_ndc_tmp(0, width), to_ndc_tmp(min_height, height),
            to_ndc_tmp(width, width), to_ndc_tmp(height, height),
            to_ndc_tmp(width, width), to_ndc_tmp(min_height, height),

    };
    GLVao cursor_vao{};
    cursor_vao.init();
    cursor_vao.bind();
    //cursor_vao.add_buffer(cursor_vertices, sizeof(cursor_vertices), 2, sizeof(vec2), 0, 0);
    //cursor_vao.load_buffers();

    gl->draw_arrays(GL_TRIANGLES, 0, 6);
    cursor_vao.destroy();
}

#endif //HOT_RELOAD_OPENGL_CLI_H
