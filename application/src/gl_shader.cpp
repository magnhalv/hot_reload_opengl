#include <glad/gl.h>
#include <cassert>
#include <cstdio>
#include <string>

#include "application.h"
#include "gl_shader.h"

auto print_shader_source(const char* text) -> void
{
    int line = 1;

    printf("\n%3i: ", line);

    while (text && *text++)
    {
        if (*text == '\n')
        {
            printf("\n%3i: ", ++line);
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

auto read_shader_file(const char* fileName) -> char*
{
    FILE* file = fopen(fileName, "r");

    if (!file)
    {
        log_error("I/O error. Cannot open shader file '%s'\n", fileName);
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

auto compile_shader(const char* path) -> u32
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
        print_shader_source(text);
        log_error("%sFile: %s\n", buffer, path);
        return Gl_Invalid_Id;
    }
    return handle;
}

auto print_program_info_log(GLuint handle) -> void
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

auto create_program(const char *vertex_path, const char *fragment_path) -> u32 {
    auto handle = gl->create_program();
    const auto vertex_handle = compile_shader(vertex_path);
    if (vertex_handle == Gl_Invalid_Id) {
        return Gl_Invalid_Id;
    }

    const auto fragment_handle = compile_shader(fragment_path);
    if (fragment_handle == Gl_Invalid_Id) {
        return Gl_Invalid_Id;
    }

    const auto vertex_length = strlen(vertex_path);
    const auto fragment_length = strlen(fragment_path);

    assert(vertex_length < Shader_Path_Max_Length);
    assert(fragment_length < Shader_Path_Max_Length);

    gl->attach_shader(handle, vertex_handle);
    gl->attach_shader(handle, fragment_handle);
    // TODO: Check for linking errors
    gl->link_program(handle);

    char buffer[8192];
    GLsizei length = 0;
    gl->get_program_info_log(handle, sizeof(buffer), &length, buffer);
    if (length)
    {
        gl->delete_program(handle);
        log_error("Failed to link shader program.\n%s\n", buffer);
        return Gl_Invalid_Id;
    }

    return handle;
}

auto GLShaderProgram::initialize(const char *vertex_path, const char *fragment_path) -> bool
{
    const auto vertex_length = strlen(vertex_path);
    const auto fragment_length = strlen(fragment_path);

    assert(vertex_length < Shader_Path_Max_Length);
    assert(fragment_length < Shader_Path_Max_Length);

    const auto handle = create_program(vertex_path, fragment_path);

    if (handle == Gl_Invalid_Id) {
        log_error("Failed to create shader program.");
        return false;
    }

    handle_ = handle;
    strcpy_s(vertex.path, Shader_Path_Max_Length, vertex_path);
    strcpy_s(fragment.path, Shader_Path_Max_Length, fragment_path);
    vertex.last_modified = platform->get_file_last_modified(vertex_path);
    fragment.last_modified = platform->get_file_last_modified(fragment_path);
    return true;
}

auto GLShaderProgram::relink_if_changed() -> void {
    auto vertex_last_modified = platform->get_file_last_modified(vertex.path);
    auto fragment_last_modified = platform->get_file_last_modified(fragment.path);
    if (vertex_last_modified > vertex.last_modified || fragment_last_modified > fragment.last_modified) {
        const auto new_handle = create_program(vertex.path, fragment.path);
        if (new_handle != Gl_Invalid_Id) {
            gl->delete_program(handle_);
            handle_ = new_handle;
        }
        vertex.last_modified = vertex_last_modified;
        fragment.last_modified = fragment_last_modified;
    }
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

int ends_with(const char* s, const char* part)
{
    return (strstr( s, part ) - s) == (strlen( s ) - strlen( part ));
}

GLenum GLShaderType_from_file_name(const char* file_name)
{
    if (ends_with(file_name, ".vert")) {
        return GL_VERTEX_SHADER;
    }
    if (ends_with(file_name, ".frag")) {
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

void GLShaderProgram::useProgram() const
{
    assert(handle_ != 0);
    gl->use_program(handle_);
}

auto GLVao::add_buffer(GLBuffer buffer) -> bool {
    if (num_buffers == Max_Buffers) {
        return false;
    }
    buffers[num_buffers++] = buffer;
    return true;
}

auto GLVao::add_uniform_buffer(GLUniformBuffer uniform_buffer) -> bool {
    if (num_uniform_buffers == Max_Buffers) {
        return false;
    }
    uniform_buffers[num_uniform_buffers++] = uniform_buffer;
    return true;
}

auto GLVao::load_buffers() -> void {
    for (i32 i = 0; i < num_buffers; i++) {
        auto &buffer = buffers[i];
        gl->create_buffers(1, &buffer.handle);
        // Populates the buffer
        gl->named_buffer_storage(buffer.handle, buffer.size, buffer.data, 0);
        // TODO: handle offset (the 0)
        gl->vertex_array_vertex_buffer(handle, buffer.index, buffer.handle, 0, buffer.stride);
        // Enables vertex attribute 1
        gl->enable_vertex_array_attrib(handle, buffer.index);
        gl->vertex_array_attrib_format(handle, buffer.index, 3, GL_FLOAT, GL_FALSE, 0);
        // Makes vertex attribute available in shader layout=buffer.index
        gl->vertex_array_attrib_binding(handle, buffer.index, buffer.index);
    }

    for (i32 i = 0; i < num_uniform_buffers; i++) {
        auto u_buf = uniform_buffers[i];
        gl->create_buffers(1, &u_buf.handle);
        gl->named_buffer_storage(u_buf.handle, u_buf.size, nullptr, GL_DYNAMIC_STORAGE_BIT);
        gl->bind_buffer_base(GL_UNIFORM_BUFFER, u_buf.index, u_buf.handle);
    }
}

auto GLVao::update_dynamic_buffers() -> void {
    for (i32 i = 0; i < num_uniform_buffers; i++) {
        auto u_buf = uniform_buffers[i];
        gl->named_buffer_sub_data(u_buf.handle, 0, u_buf.size, u_buf.data);
    }
}