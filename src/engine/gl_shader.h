#pragma once

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <platform/types.h>

#include "assets.h"

const i32 Shader_Path_Max_Length = 512;
const u32 Max_Buffers = 2;

struct Shader {
    char path[Shader_Path_Max_Length];
    TimeStamp last_modified;
};

struct GLUniformBuffer {
    u32 handle;
    void *data;
    u32 index;
    GLsizeiptr size;
    GLbitfield flags;
};

struct GLShaderProgram
{
public:
    auto initialize(const char *vertex_path, const char *fragment_path) -> bool;
    auto useProgram() const -> void;
    auto free() -> void;
    [[nodiscard]] GLuint getHandle() const { return _handle; }

    auto set_uniform(const char *name, const glm::vec4 &vec) const -> void;
    auto relink_if_changed() -> void;

    auto add_uniform_buffer(void *data, GLsizeiptr size, u32 index, GLbitfield flags) -> bool;
    auto update_dynamic_buffers() const -> void;

private:
    auto create_program(const char *vertex_path, const char *fragment_path) -> u32;

    FileAsset _vertex_source{};
    FileAsset _fragment_source{};
    GLUniformBuffer _uniform_buffers[Max_Buffers]{};
    u32 _num_uniform_buffers{};

    GLuint _handle{};
};

struct GLBuffer {
    u32 handle;
    void *data;
    u32 index;
    i32 stride;
    GLsizeiptr size;
    GLbitfield flags;
};

struct GLVao {
    u32 handle;
    GLBuffer buffers[Max_Buffers];
    u32 num_buffers;

    auto init() -> void;
    auto destroy() -> void;
    auto bind() const -> void;

    [[nodiscard]] auto add_buffer(void *data, GLsizeiptr size, u32 index, i32 stride, GLbitfield flags) -> bool;
    auto load_buffers() -> void;
};

GLenum GLShaderType_from_file_name(const char* file_name);
