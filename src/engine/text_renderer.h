#ifndef HOT_RELOAD_OPENGL_TEXT_RENDERER_H
#define HOT_RELOAD_OPENGL_TEXT_RENDERER_H

#include "gl/gl_vao.h"
#include "array.h"
#include "gl/gl_shader.h"

struct Character {
    u32 texture_id{};
    ivec2 size;
    ivec2 bearing;
    u32 advance{};
};

struct TextRenderer {
    auto load_font(const char *path, MemoryArena &permanent_arena) -> void;
    auto init(GLShaderProgram *program) -> void;
    auto render(const char *text, i32 length, f32 x, f32 y, f32 scale, const mat4 &ortho_projection) -> void;

private:
    GLVao _vao;
    GLShaderProgram *_program;
    Array<Character> _characters;
};

#endif //HOT_RELOAD_OPENGL_TEXT_RENDERER_H
