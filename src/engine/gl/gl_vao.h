#ifndef HOT_RELOAD_OPENGL_GL_VAO_H
#define HOT_RELOAD_OPENGL_GL_VAO_H

#include "gl.h"

struct GLBufferElementDescription {
    u32 location;
    i32 size;
    i32 offset;
    i32 stride;
};

struct GLBuffer {
    const static u32 Max_Num_Descriptions = 5;

    u32 handle;
    GLsizeiptr size;
    GLbitfield flags;
    GLBufferElementDescription description[Max_Num_Descriptions];
    i32 num_descriptions;
};

struct GLVao {
    static const i32 Max_Buffers = 5;

    u32 handle;
    GLBuffer buffers[Max_Buffers];
    u32 num_buffers;

    auto init() -> void;

    auto destroy() -> void;

    auto bind() const -> void;

    auto add_buffer(GLsizeiptr size, GLbitfield flags = 0) -> void;
    // TODO: Move to buffer, and make offset last argument with default = 0;
    auto upload_buffer_data(i32 buffer_idx, void* data, GLsizeiptr offset, GLsizeiptr size) -> void;
    auto add_buffer_desc(u32 buffer_idx, u32 location, i32 size, i32 offset, i32 stride) -> void;
    auto add_buffer_desc(u32 buffer_idx, GLBufferElementDescription desc) -> void;
    auto upload_buffer_desc() -> void;

};

#endif //HOT_RELOAD_OPENGL_GL_VAO_H
