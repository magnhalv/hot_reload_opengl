#ifndef HOT_RELOAD_OPENGL_CLI_H
#define HOT_RELOAD_OPENGL_CLI_H

#include <platform/types.h>

#include "gl/gl.h"
#include "gl/gl_vao.h"
#include "gl/gl_shader.h"

struct Cli {
    static const i32 Max_Height = 200;
    static constexpr f32 Duration = 1.0f;

    GLVao _vao;
    GLShaderProgram *_single_color_program;

    f32 t;
    f32 _current_height;

    auto init(GLShaderProgram *single_color_program) -> void;
    auto update(f32 client_width, f32 client_height, f32 dt) -> void;
    auto render(f32 client_width, f32 client_height) -> void;

};


#endif //HOT_RELOAD_OPENGL_CLI_H
