#pragma once

#include <math/mat4.h>
#include <math/vec4.h>
#include <platform/types.h>

#include "array.h"
#include "camera.h"
#include "cli/cli.h"
#include "framebuffer.h"
#include "gl/gl_shader.h"
#include "gl/gl_vao.h"
#include "memory_arena.h"
#include "mesh.h"
#include "model.h"
#include "options.hpp"
#include "text_renderer.h"

enum class PointerMode { NORMAL = 0, LOOK_AROUND, GRAB };

/// Processed mouse input
struct Pointer {
  f32 x = 0;
  f32 y = 0;

  vec3 ray; // Mouse ray in world space from the camera enabled at the start of the frame
  auto update_pos(const MouseRaw& raw, i32 client_width, i32 client_height) -> void;
  auto update_ray(const mat4& view, const mat4& inv_projection, i32 client_width, i32 client_height) -> void;
};

struct Window {
  f32 width;
  f32 height;
  mat4 ortho;
  mat4 perspective;
};

extern GraphicsOptions* graphics_options;

struct PerFrameData {
  mat4 projection;
  mat4 view;
  mat4 model;
};

struct EngineState {
  Pointer pointer;
  bool is_initialized = false;
  Array<Model> models;
  Mesh floor;
  Camera camera;
  MemoryArena transient;
  MemoryArena permanent;

  GLGlobalUniformBufferContainer uniform_buffer_container;
  Framebuffer framebuffer;
  MultiSampleFramebuffer ms_framebuffer;
  GLVao quad_vao{};

  bool is_cli_enabled;

  // Gameplay
  PointerMode pointer_mode;
  Cli cli;

  GraphicsOptions graphics_options;

  TextRenderer text_renderer;
  Font* font;
};

extern "C" __declspec(dllexport) void update_and_render(EngineMemory* memory, EngineInput* app_input);
extern "C" __declspec(dllexport) void load(GLFunctions* in_gl, Platform* in_platform, EngineMemory* in_memory);
