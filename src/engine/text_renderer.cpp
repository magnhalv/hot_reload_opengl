#include "text_renderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "logger.h"
#include "memory_arena.h"

auto TextRenderer::load_font(const char* path, MemoryArena& permanent_arena) -> void {
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    crash_and_burn("Failed to initialize FreeType library.");
  }

  FT_Face face;
  if (FT_New_Face(ft, path, 0, &face)) {
    crash_and_burn("Failed to load Ubuntu font.");
  }
  FT_Set_Pixel_Sizes(face, 0, 48);

  gl->pixel_store_i(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

  _characters.init(permanent_arena, 128);
  i32 i = 0;
  for (u8 c = 0; c < 128; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      crash_and_burn("FreeType: Failed to load glyph.");
    }
    // generate texture
    u32 texture;
    gl->gen_textures(1, &texture);
    gl->bind_texture(GL_TEXTURE_2D, texture);
    gl->tex_image_2d(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
        GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    // set texture options
    gl->tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // now store character for later use
    _characters[i++] = { texture, ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), static_cast<u32>(face->glyph->advance.x) };
  }
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

auto TextRenderer::init(GLShaderProgram* program) -> void {
  _program = program;
  _program->initialize(R"(.\assets\shaders\font.vert)", R"(.\assets\shaders\font.frag)");

  _vao.init();
  _vao.add_buffer(sizeof(f32) * 6 * 4);
  _vao.add_buffer_desc(0, GLBufferElementDescription{ .location = 0, .size = 4, .offset = 0, .stride = 4 * sizeof(f32) });
  _vao.upload_buffer_desc();
}

auto TextRenderer::render(const char* text, i32 length, f32 x, f32 y, f32 scale, const mat4& ortho_projection) -> void {
  assert(_program != nullptr);

  // activate corresponding render state
  _vao.bind();
  _program->useProgram();
  _program->set_uniform("projection", ortho_projection);
  _program->set_uniform("text_color", vec3(0.8, 0.8, 0.8));
  gl->active_texture(GL_TEXTURE0);

  // iterate through all characters

  gl->enable(GL_BLEND);
  gl->disable(GL_DEPTH_TEST);
  gl->blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (auto i = 0; i < length; i++) {
    char c = text[i];
    if (c == '\0') {
      continue;
    }
    assert(c > 0 && c < _characters.size());
    Character ch = _characters[c];

    f32 x_pos = x + ch.bearing.x * scale;
    f32 y_pos = y - (ch.size.y - ch.bearing.y) * scale;

    f32 w = ch.size.x * scale;
    f32 h = ch.size.y * scale;

    f32 vertices[6][4] = { { x_pos, y_pos + h, 0.0f, 0.0f }, { x_pos, y_pos, 0.0f, 1.0f }, { x_pos + w, y_pos, 1.0f, 1.0f },

      { x_pos, y_pos + h, 0.0f, 0.0f }, { x_pos + w, y_pos, 1.0f, 1.0f }, { x_pos + w, y_pos + h, 1.0f, 0.0f } };
    // render glyph texture over quad
    gl->bind_texture(GL_TEXTURE_2D, ch.texture_id);
    // update content of VBO memory

    _vao.upload_buffer_data(0, vertices, 0, sizeof(vertices));
    // render quad
    gl->draw_arrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
  }
  gl->bind_vertex_array(0);
  gl->bind_texture(GL_TEXTURE_2D, 0);
  gl->disable(GL_BLEND);
  gl->disable(GL_DEPTH_TEST);
}
