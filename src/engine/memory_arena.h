#ifndef HOT_RELOAD_OPENGL_MEMORY_ARENA_H
#define HOT_RELOAD_OPENGL_MEMORY_ARENA_H

#include <platform/types.h>

#include "logger.h"

const u32 GUARD_PATTERN = 0xEFBEADDE; // DEADBEEF

struct ArenaGuard {
    u32 guard_pattern;
    ArenaGuard *next;
    ArenaGuard *prev;
};

struct MemoryArena {
    u8* memory = nullptr;
    u64 used = 0;
    u64 size = 0;

    auto init(void *in_memory, u32 in_size) -> void;
    auto allocate(u64 request_size) -> void*;
    auto allocate_arena(u64 request_size) -> MemoryArena*;
    auto clear() -> void;
    auto check_integrity() const -> void;
};

template <typename T>
auto inline allocate(MemoryArena &arena, i32 num = 1) -> T* {
    return static_cast<T*>(arena.allocate(sizeof(T) * num));
}

extern MemoryArena *g_transient; // This one is erased every frame.

void set_transient_arena(MemoryArena *arena);
void clear_transient();
void* allocate_transient(u64 request_size);
#if ENGINE_TEST
void unset_transient_arena();
#endif

#endif //HOT_RELOAD_OPENGL_MEMORY_ARENA_H
