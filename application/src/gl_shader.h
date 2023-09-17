#ifndef HOT_RELOAD_OPENGL_GL_SHADER_H
#define HOT_RELOAD_OPENGL_GL_SHADER_H

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <platform/types.h>

const i32 Shader_Path_Max_Length = 512;

struct GLShaderProgram
{
public:
    void initialize(const char *a_path, const char *b_path);
    void useProgram() const;
    void free();
    [[nodiscard]] GLuint getHandle() const { return handle_; }

    void set_uniform(const char *name, const glm::vec4 &vec) const;

private:
    char _a_path[Shader_Path_Max_Length];
    char _b_path[Shader_Path_Max_Length];
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
