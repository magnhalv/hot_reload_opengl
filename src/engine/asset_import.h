#pragma once

#include "memory_arena.h"
#include "mesh.h"

auto import_model(const char* path, Model& model, MemoryArena& storage) -> void;
