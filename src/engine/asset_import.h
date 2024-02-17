#pragma once

#include "array.h"
#include "memory_arena.h"
#include "model.h"

auto import_model(const char* path, MemoryArena& storage) -> Array<Model>;
