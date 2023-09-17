#include <cstdio>
#include <glad/gl.h>

#include "application.h"
#include "gl_shader.h"
#include "asset_import.h"

GLFunctions *gl = nullptr;

static const char* shaderCodeVertex = R"(
#version 460 core
layout (location=0) out vec3 color;
const vec2 pos[3] = vec2[3](
	vec2(-0.6, -0.4),
	vec2( 0.6, -0.4),
	vec2( 0.0,  0.6)
);
const vec3 col[3] = vec3[3](
	vec3( 1.0, 0.0, 0.0 ),
	vec3( 0.0, 1.0, 0.0 ),
	vec3( 0.0, 0.0, 1.0 )
);
void main()
{
	gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
	color = col[gl_VertexID];
}
)";

static const char* shaderCodeFragment = R"(
#version 460 core
layout (location=0) in vec3 color;
layout (location=0) out vec4 out_FragColor;
void main()
{
	out_FragColor = vec4(color, 1.0);
};
)";

void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input) {
    assert(sizeof(AppState) < memory->permanent_storage_size);
    auto *state = (AppState*)memory->permanent_storage;
    const f32 ratio = static_cast<f32>(app_input->client_width) / static_cast<f32>(app_input->client_height);

    auto *mesh = &state->mesh;
    
    [[unlikely]]
    if (!state->is_initialized) {
        state->transient.size = memory->transient_storage_size;
        state->transient.used = 0;
        state->transient.memory = (u8*)memory->transient_storage;
        set_transient_arena(&state->transient);

        import_mesh("assets/meshes/cube.glb", &state->mesh);

        GLShader vertex(R"(.\assets\shaders\mesh.vert)");
        GLShader frag(R"(.\assets\shaders\basic_light.frag)");
        state->program.initialize(vertex, frag);
        state->program.useProgram();
        state->camera.init(-90.0f, 0.0f, glm::vec3(0.0f, 1.0f, 5.0f));

        auto data_size = static_cast<GLsizeiptr>(sizeof(glm::vec3) * mesh->num_vertices); // NOLINT
        assert(mesh->num_vertices == mesh->num_normals);
        gl->create_vertex_arrays(1, &mesh->vao);
        gl->bind_vertex_array(mesh->vao);

        gl->create_buffers(1, &mesh->vertices_vbo);
        // Populates the buffer
        gl->named_buffer_storage(mesh->vertices_vbo, data_size, mesh->vertices, 0);
        // Binds the buffer to binding point 0 in the vao
        gl->vertex_array_vertex_buffer(mesh->vao, 0, mesh->vertices_vbo, 0, sizeof(glm::vec3));
        // Enables vertex attribute 0
        gl->enable_vertex_array_attrib(mesh->vao, 0);
        gl->vertex_array_attrib_format(mesh->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
        // Makes vertex attribute available in shader layout=0
        gl->vertex_array_attrib_binding(mesh->vao, 0, 0);

        gl->create_buffers(1, &mesh->normals_vbo);
        // Populates the buffer
        gl->named_buffer_storage(mesh->normals_vbo, data_size, mesh->normals, 0);
        // Binds the buffer to binding point 1 in the vao
        gl->vertex_array_vertex_buffer(mesh->vao, 1, mesh->normals_vbo, 0, sizeof(glm::vec3));
        // Enables vertex attribute 1
        gl->enable_vertex_array_attrib(mesh->vao, 1);
        gl->vertex_array_attrib_format(mesh->vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
        // Makes vertex attribute available in shader layout=1
        gl->vertex_array_attrib_binding(mesh->vao, 1, 1);

        gl->create_buffers(1, &mesh->mvp_vbo);
        gl->named_buffer_storage(mesh->mvp_vbo, sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
        gl->bind_buffer_base(GL_UNIFORM_BUFFER, 0, mesh->mvp_vbo);


        gl->create_buffers(1, &mesh->light_vbo);
        gl->named_buffer_storage(mesh->light_vbo, sizeof(glm::vec4), nullptr, GL_DYNAMIC_STORAGE_BIT);
        gl->bind_buffer_base(GL_UNIFORM_BUFFER, 1, mesh->light_vbo);

        gl->clear_color(0.0f, 0.0f, 0.0f, 0.0f);
        state->is_initialized = true;
    }

    state->camera.update_cursor(static_cast<f32>(app_input->input->mouse.dx), static_cast<f32>(app_input->input->mouse.dy));
    state->camera.update_keyboard(*app_input->input);

    state->program.useProgram();
    gl->viewport(0, 0, app_input->client_width, app_input->client_height);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gl->enable(GL_DEPTH_TEST);

    const glm::mat4 m = state->camera.get_view();
    const glm::mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
    glm::mat4 mvp = p * m;
    gl->named_buffer_sub_data(mesh->mvp_vbo, 0, sizeof(glm::mat4), &mvp);

    auto light =glm::vec4(0.0f, 1.0f, 4.0f, 0.0f);
    gl->named_buffer_sub_data(mesh->light_vbo, 0, sizeof(glm::vec4), &light);

    gl->polygon_mode(GL_FRONT_AND_BACK, GL_FILL);
    gl->draw_arrays(GL_TRIANGLES, 0, mesh->num_vertices);

    GLenum err;
    bool found_error = false;
    while((err = gl->get_error()) != GL_NO_ERROR)
    {
        printf("OpenGL Error %d\n", err);
        found_error = true;
    }
    if (found_error) {
        exit(1);
    }

    clear_transient();
}

void load_gl_functions(GLFunctions * in_gl) {
    gl = in_gl;
}