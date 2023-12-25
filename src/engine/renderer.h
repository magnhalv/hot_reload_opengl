#ifndef HOT_RELOAD_OPENGL_RENDERER_H
#define HOT_RELOAD_OPENGL_RENDERER_H

#include "mesh.h"
#include "engine.h"

// TODO: Rename this
auto enable_stencil_test() -> void;
auto enable_outline() -> void;
auto disable_stencil_test() -> void;
auto render_mesh(Mesh &mesh) -> void;

#endif //HOT_RELOAD_OPENGL_RENDERER_H
