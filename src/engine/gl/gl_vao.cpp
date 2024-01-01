#include "gl_vao.h"

auto GLVao::init() -> void {
    gl->create_vertex_arrays(1, &handle);
}

auto GLVao::destroy() -> void {
    gl->delete_vertex_array(1, &handle);
    gl->bind_vertex_array(0);
}

auto GLVao::bind() const -> void {
    gl->bind_vertex_array(handle);
}

auto GLVao::add_buffer(GLsizeiptr size, GLbitfield flags) -> void {
    assert(num_buffers < Max_Buffers);
    auto &buf = buffers[num_buffers++];
    buf.size = size;
    buf.flags = flags;
}

auto GLVao::upload_buffer_data(i32 buffer_idx, void* data, GLsizeiptr offset, GLsizeiptr size) -> void {
    auto &buf = buffers[buffer_idx];
    assert((size + offset) <= buf.size);
    gl->named_buffer_sub_data(buf.handle, offset, size, data);
}

auto GLVao::add_buffer_desc(u32 buffer_idx, u32 location, i32 size, i32 offset, i32 stride) -> void {
    assert(buffer_idx < num_buffers && buffer_idx >= 0);
    auto &buffer = buffers[buffer_idx];
    assert(buffer.num_descriptions < GLBuffer::Max_Num_Descriptions);
    auto &desc = buffer.description[buffer.num_descriptions++];
    desc.location = location;
    desc.size = size;
    desc.offset = offset;
    desc.stride = stride;
}

auto GLVao::add_buffer_desc(u32 buffer_idx, GLBufferElementDescription desc) -> void {
    assert(buffer_idx < num_buffers && buffer_idx >= 0);
    auto &buffer = buffers[buffer_idx];
    assert(buffer.num_descriptions < GLBuffer::Max_Num_Descriptions);
    buffer.description[buffer.num_descriptions++] = desc;
}

auto GLVao::upload_buffer_desc() -> void {
    for (i32 i = 0; i < num_buffers; i++) {
        auto &buffer = buffers[i];
        gl->create_buffers(1, &buffer.handle);
        // TODO: Dont make every buffer dynamic
        gl->named_buffer_storage(buffer.handle, buffer.size, nullptr, GL_DYNAMIC_STORAGE_BIT);

        assert(buffer.num_descriptions > 0);
        for (i32 d = 0; d < buffer.num_descriptions; d++) {
            auto &desc = buffer.description[d];
            // Attach the buffer to the VAO
            gl->vertex_array_vertex_buffer(handle, desc.location, buffer.handle, desc.offset, desc.stride);
            // Enable and specify format of vertex attribute 0
            gl->enable_vertex_array_attrib(handle, desc.location);
            gl->vertex_array_attrib_format(handle, desc.location, desc.size, GL_FLOAT, GL_FALSE, 0);
            // Makes vertex attribute available in shader layout=desc.location
            gl->vertex_array_attrib_binding(handle, desc.location, desc.location);
        }
    }
}