#include "cli.h"

auto Cli::init(GLShaderProgram *single_color_program) -> void {
    _vao.init();
    _vao.bind();
    _vao.add_buffer(2*6*sizeof(f32));
    _vao.add_buffer_desc(0, 0, 2, 0, 2*sizeof(f32));
    _vao.upload_buffer_desc();

    _single_color_program = single_color_program;
}

auto Cli::update(f32 client_width, f32 client_height, f32 dt) -> void {
    t += dt;
    t = t < Duration ? t : Duration;
    auto progress = t/Duration;

    progress--;
    progress = progress * progress * progress + 1;

    auto target_height = client_height/2;
    _current_height = client_height - target_height * progress;
}

auto Cli::render(f32 client_width, f32 client_height) -> void {
    _vao.bind();
    _single_color_program->useProgram();
    f32 cursor_vertices[] = {
            0.0f, _current_height,
            client_width, client_height,
            0.0f, client_height,

            0.0f, _current_height,
            client_width, client_height,
            client_width, _current_height,

    };

    _vao.upload_buffer_data(0, cursor_vertices, 0, sizeof(cursor_vertices));
    gl->draw_arrays(GL_TRIANGLES, 0, 6);
}
