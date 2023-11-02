#ifndef HOT_RELOAD_OPENGL_MEMORY_ARENA_H
#define HOT_RELOAD_OPENGL_MEMORY_ARENA_H

#include <platform/types.h>

#include "logger.h"

struct MemoryArena {
    u8* memory = nullptr;
    u64 used = 0;
    u64 size = 0;

    auto init(void *in_memory, u32 in_size) -> void;
    auto allocate(u64 request_size) -> void*;
    auto clear() -> void;
    auto check_integrity() -> void;
};

extern MemoryArena *transient; // This one is erased every frame.

void set_transient_arena(MemoryArena *arena);

void clear_transient();

void* allocate_transient(u64 request_size);

#endif //HOT_RELOAD_OPENGL_MEMORY_ARENA_H
