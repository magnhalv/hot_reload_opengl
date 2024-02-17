#pragma once

#include "memory_arena.h"
#include "model.h"

auto import_model(const char* path, Model& model, MemoryArena& storage) -> void;
