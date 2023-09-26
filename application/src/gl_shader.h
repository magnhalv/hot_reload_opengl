#ifndef HOT_RELOAD_OPENGL_GL_SHADER_H
#define HOT_RELOAD_OPENGL_GL_SHADER_H

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <platform/types.h>

const i32 Shader_Path_Max_Length = 512;

struct Shader {
    char path[Shader_Path_Max_Length];
    TimeStamp last_modified;
};

struct GLShaderProgram
{
public:
    auto initialize(const char *vertex_path, const char *fragment_path) -> bool;
    auto useProgram() const -> void;
    auto free() -> void;
    [[nodiscard]] GLuint getHandle() const { return handle_; }

    auto set_uniform(const char *name, const glm::vec4 &vec) const -> void;
    auto relink_if_changed() -> void;
private:
    Shader vertex;
    Shader fragment;
    GLuint handle_;

};

class GLBuffer
{
public:
    GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags);
    ~GLBuffer();

    [[nodiscard]] GLuint getHandle() const { return handle_; }

private:
    GLuint handle_;
};

GLenum GLShaderType_from_file_name(const char* file_name);

#endif //HOT_RELOAD_OPENGL_GL_SHADER_H
