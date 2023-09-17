#include <glad/gl.h>
#include <cassert>
#include <cstdio>
#include <string>

#include "application.h"
#include "gl_shader.h"

void print_shader_source(const char* text)
{
    int line = 1;

    printf("\n(%3i) ", line);

    while (text && *text++)
    {
        if (*text == '\n')
        {
            printf("\n(%3i) ", ++line);
        }
        else if (*text == '\r')
        {
        }
        else
        {
            printf("%c", *text);
        }
    }

    printf("\n");
}

char* read_shader_file(const char* fileName)
{
    FILE* file = fopen(fileName, "r");

    if (!file)
    {
        printf("I/O error. Cannot open shader file '%s'\n", fileName);
        return {};
    }
    // TODO: Use platform read file function
    fseek(file, 0L, SEEK_END);
    const auto bytes_in_file = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* buffer = (char*)allocate_transient(bytes_in_file + 1);
    const size_t bytes_read = fread(buffer, 1, bytes_in_file, file);
    fclose(file);

    buffer[bytes_read] = 0;

    static constexpr unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };

    if (bytes_read > 3)
    {
        if (!memcmp(buffer, BOM, 3))
            memset(buffer, ' ', 3);
    }

    return buffer;
    /*std::string code(buffer);

    while (code.find("#include ") != std::string::npos)
    {
        const auto pos = code.find("#include ");
        const auto p1 = code.find('<', pos);
        const auto p2 = code.find('>', pos);
        if (p1 == std::string::npos || p2 == std::string::npos || p2 <= p1)
        {
            printf("Error while loading shader program: %s\n", code.c_str());
            return {};
        }
        const std::string name = code.substr(p1 + 1, p2 - p1 - 1);
        const std::string include = read_shader_file(name.c_str());
        code.replace(pos, p2-pos+1, include.c_str());
    }

    return code;*/
}

u32 compile_shader(const char* path)
{
    auto type = GLShaderType_from_file_name(path);
    auto text = read_shader_file(path);
    auto handle = gl->create_shader(type);
    gl->shader_source(handle, 1, &text, nullptr);
    gl->compile_shader(handle);

    char buffer[8192];
    GLsizei length = 0;
    gl->get_shader_info_log(handle, sizeof(buffer), &length, buffer);

    if (length)
    {
        printf("%s (File: %s)\n", buffer, path);
        print_shader_source(text);
        assert(false);
    }
    return handle;
}

void print_program_info_log(GLuint handle)
{
    char buffer[8192];
    GLsizei length = 0;
    gl->get_program_info_log(handle, sizeof(buffer), &length, buffer);
    if (length)
    {
        printf("%s\n", buffer);
        assert(false);
    }
}

void GLShaderProgram::initialize(const char *a_path, const char *b_path)
{
    handle_ = gl->create_program();
    const auto a = compile_shader(a_path);
    const auto b = compile_shader(b_path);

    const auto a_length = strlen(a_path);
    const auto b_length = strlen(b_path);

    assert(a_length < Shader_Path_Max_Length);
    assert(b_length < Shader_Path_Max_Length);

    strcpy_s(_a_path, Shader_Path_Max_Length, a_path);
    strcpy_s(_b_path, Shader_Path_Max_Length, b_path);

    gl->attach_shader(handle_, a);
    gl->attach_shader(handle_, b);
    gl->link_program(handle_);
    print_program_info_log(handle_);
}


void GLShaderProgram::set_uniform(const char *name, const glm::vec4 &vec) const {
    i32 id = gl->get_uniform_location(handle_, name);
    gl->uniform_4f(id, vec.x, vec.y, vec.z, vec.w);
}

void GLShaderProgram::free()
{
    gl->delete_program(handle_);
    handle_ = 0;
}

void GLShaderProgram::useProgram() const
{
    assert(handle_ != 0);
    gl->use_program(handle_);
}

GLBuffer::GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags)
{
    gl->create_buffers(1, &handle_);
    gl->named_buffer_storage(handle_, size, data, flags);
}

GLBuffer::~GLBuffer()
{
    assert(handle_ != 0);
    gl->delete_buffers(1, &handle_);
}

int ends_with(const char* s, const char* part)
{
    return (strstr( s, part ) - s) == (strlen( s ) - strlen( part ));
}

GLenum GLShaderType_from_file_name(const char* file_name)
{
    if (ends_with(file_name, ".vert")) {
        printf("Vert\n");
        return GL_VERTEX_SHADER;
    }
    if (ends_with(file_name, ".frag")) {
        printf("Frag\n");
        return GL_FRAGMENT_SHADER;
    }
    if (ends_with(file_name, ".geom"))
        return GL_GEOMETRY_SHADER;

    if (ends_with(file_name, ".tesc"))
        return GL_TESS_CONTROL_SHADER;

    if (ends_with(file_name, ".tese"))
        return GL_TESS_EVALUATION_SHADER;

    if (ends_with(file_name, ".comp"))
        return GL_COMPUTE_SHADER;

    assert(false);

    return 0;
}
