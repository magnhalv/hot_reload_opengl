#include <glad/gl.h>
#include <assert.h>
#include <stdio.h>
#include <string>

#include "gl_shader.h"

GLShader::GLShader(const char* fileName)
        : GLShader(GLShaderTypeFromFileName(fileName), readShaderFile(fileName).c_str(), fileName)
{}

GLShader::GLShader(GLenum type, const char* text, const char* debugFileName)
        : type_(type)
        , handle_(glCreateShader(type))
{
    glShaderSource(handle_, 1, &text, nullptr);
    glCompileShader(handle_);

    char buffer[8192];
    GLsizei length = 0;
    glGetShaderInfoLog(handle_, sizeof(buffer), &length, buffer);

    if (length)
    {
        printf("%s (File: %s)\n", buffer, debugFileName);
        printShaderSource(text);
        assert(false);
    }
}

GLShader::~GLShader()
{
    glDeleteShader(handle_);
}

void printProgramInfoLog(GLuint handle)
{
    char buffer[8192];
    GLsizei length = 0;
    glGetProgramInfoLog(handle, sizeof(buffer), &length, buffer);
    if (length)
    {
        printf("%s\n", buffer);
        assert(false);
    }
}

void GLProgram::initialize(const GLShader& a)
{
    assert(handle_ == 0);
    handle_ = glCreateProgram();
    glAttachShader(handle_, a.getHandle());
    glLinkProgram(handle_);
    printProgramInfoLog(handle_);
}

void GLProgram::initialize(const GLShader& a, const GLShader& b)
{
    assert(handle_ == 0);
    handle_ = glCreateProgram();
    glAttachShader(handle_, a.getHandle());
    glAttachShader(handle_, b.getHandle());
    glLinkProgram(handle_);
    printProgramInfoLog(handle_);
}

void GLProgram::initialize(const GLShader& a, const GLShader& b, const GLShader& c)
{
    assert(handle_ == 0);
    handle_ = glCreateProgram();
    glAttachShader(handle_, a.getHandle());
    glAttachShader(handle_, b.getHandle());
    glAttachShader(handle_, c.getHandle());
    glLinkProgram(handle_);
    printProgramInfoLog(handle_);
}

void GLProgram::initialize(const GLShader& a, const GLShader& b, const GLShader& c, const GLShader& d, const GLShader& e)
{
    assert(handle_ == 0);
    handle_ = glCreateProgram();
    glAttachShader(handle_, a.getHandle());
    glAttachShader(handle_, b.getHandle());
    glAttachShader(handle_, c.getHandle());
    glAttachShader(handle_, d.getHandle());
    glAttachShader(handle_, e.getHandle());
    glLinkProgram(handle_);
    printProgramInfoLog(handle_);
}

void GLProgram::set_uniform(const char *name, const glm::vec4 &vec) const {
    i32 id = glGetUniformLocation(handle_, name);
    glUniform4f(id, vec.x, vec.y, vec.z, vec.w);
}

GLProgram::~GLProgram()
{
    glDeleteProgram(handle_);
}

void GLProgram::useProgram() const
{
    assert(handle_ != 0);
    glUseProgram(handle_);
}

GLBuffer::GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags)
{
    glCreateBuffers(1, &handle_);
    glNamedBufferStorage(handle_, size, data, flags);
}

GLBuffer::~GLBuffer()
{
    assert(handle_ != 0);
    glDeleteBuffers(1, &handle_);
}

void GLShader::printShaderSource(const char* text)
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

std::string GLShader::readShaderFile(const char* fileName)
{
    FILE* file = fopen(fileName, "r");

    if (!file)
    {
        printf("I/O error. Cannot open shader file '%s'\n", fileName);
        return std::string();
    }

    fseek(file, 0L, SEEK_END);
    const auto bytesinfile = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* buffer = (char*)alloca(bytesinfile + 1);
    const size_t bytesread = fread(buffer, 1, bytesinfile, file);
    fclose(file);

    buffer[bytesread] = 0;

    static constexpr unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };

    if (bytesread > 3)
    {
        if (!memcmp(buffer, BOM, 3))
            memset(buffer, ' ', 3);
    }

    std::string code(buffer);

    while (code.find("#include ") != code.npos)
    {
        const auto pos = code.find("#include ");
        const auto p1 = code.find('<', pos);
        const auto p2 = code.find('>', pos);
        if (p1 == code.npos || p2 == code.npos || p2 <= p1)
        {
            printf("Error while loading shader program: %s\n", code.c_str());
            return std::string();
        }
        const std::string name = code.substr(p1 + 1, p2 - p1 - 1);
        const std::string include = readShaderFile(name.c_str());
        code.replace(pos, p2-pos+1, include.c_str());
    }

    return code;
}
