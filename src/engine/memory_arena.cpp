#include <cassert>
#include <cstdlib>
#include <cstring>

#include "memory_arena.h"
#include "memory.h"

MemoryArena *g_transient = nullptr;

auto MemoryArena::allocate(u64 request_size) -> void * {
    assert(memory != nullptr);
    if (size < used + request_size + sizeof(ArenaGuard)) {
        crash_and_burn("Failed to allocate %" PRIu64 " bytes. Only %" PRIu64 " remaining.", request_size, size - used - sizeof(ArenaGuard));
    }

    auto *previous_guard = reinterpret_cast<ArenaGuard *>(&memory[used - sizeof(ArenaGuard)]);
    assert(previous_guard->guard_pattern == GUARD_PATTERN);

    memset(&memory[used], 0, request_size);

    void *result = &memory[used];
    used += request_size + sizeof(ArenaGuard);

    auto *new_guard = reinterpret_cast<ArenaGuard *>(&memory[used - sizeof(ArenaGuard)]);
    new_guard->guard_pattern = GUARD_PATTERN;
    new_guard->next = nullptr;
    new_guard->prev = previous_guard;

    previous_guard->next = new_guard;


    log_info("MemoryArena: allocated %llu bytes. Capacity: %.2f %%.", request_size,
             (static_cast<f32>(used)*100.0f)/ static_cast<f32>(size));

    return result;
}

auto MemoryArena::allocate_arena(u64 request_size) -> MemoryArena* {
    void *mem_block = static_cast<u8*>(allocate(request_size + sizeof(MemoryArena)));
    auto *new_arena = static_cast<MemoryArena*>(mem_block);
    new_arena->init(static_cast<u8*>(mem_block) + sizeof(MemoryArena), request_size);
    return new_arena;
}

auto MemoryArena::clear() -> void {
    debug_set_memory(memory, size);
    used = sizeof(ArenaGuard);
    auto *first_guard = reinterpret_cast<ArenaGuard *>(memory);
    first_guard->guard_pattern = GUARD_PATTERN;
    first_guard->next = nullptr;
}

auto MemoryArena::check_integrity() const -> void {
    auto *guard = reinterpret_cast<ArenaGuard *>(memory);

    if (guard == nullptr) {
        crash_and_burn("MemoryArena: is not initialized. Always initialize arenas before use!");
    }

    i32 guard_index = 0;
    if (guard->guard_pattern != GUARD_PATTERN) {
        crash_and_burn("MemoryArena: integrity check failed at guard index %d", guard_index);
    }

    while (guard->next != nullptr) {
        guard = guard->next;
        guard_index++;
        if (guard->guard_pattern != GUARD_PATTERN) {
            crash_and_burn("MemoryArena: integrity check failed at guard index %d", guard_index);
        }
    }
}

auto MemoryArena::init(void *in_memory, u32 in_size) -> void {
    memory = static_cast<u8 *>(in_memory);
    size = in_size;
    clear();
}


void set_transient_arena(MemoryArena *arena) {
    assert(arena->memory != nullptr);
    assert(g_transient == nullptr);
    g_transient = arena;
}

#if ENGINE_TEST
void unset_transient_arena() {
    g_transient = nullptr;
}
#endif

void clear_transient() {
    assert(g_transient != nullptr);
    g_transient->clear();
}

void *allocate_transient(u64 request_size) {
    return g_transient->allocate(request_size);
}