#include <cassert>
#include <cstdlib>
#include <cstring>

#include "memory_arena.h"
#include "memory.h"

MemoryArena *transient = nullptr;
const u32 GUARD_PATTERN = 0xEFBEADDE; // DEADBEEF

struct ArenaGuard {
    u32 guard_pattern;
    ArenaGuard *next;
};

auto MemoryArena::allocate(u64 request_size) -> void * {
    assert(size >= request_size + sizeof(ArenaGuard));
    auto *previous_guard = reinterpret_cast<ArenaGuard *>(&memory[used - sizeof(ArenaGuard)]);
    assert(previous_guard->guard_pattern == GUARD_PATTERN);

    memset(&memory[used], 0, request_size);

    void *result = &memory[used];
    used += request_size + sizeof(ArenaGuard);
    auto *new_guard = reinterpret_cast<ArenaGuard *>(&memory[used - sizeof(ArenaGuard)]);
    new_guard->guard_pattern = GUARD_PATTERN;
    new_guard->next = nullptr;
    previous_guard->next = new_guard;


    log_info("MemoryArena: allocated %llu bytes. %f %", request_size,
             (static_cast<f32>(used)*100.0f)/ static_cast<f32>(size));

    return result;
}

auto MemoryArena::clear() -> void {
    debug_set_memory(memory, size);
    used = sizeof(ArenaGuard);
    auto *first_guard = reinterpret_cast<ArenaGuard *>(&memory[used - sizeof(ArenaGuard)]);
    first_guard->guard_pattern = GUARD_PATTERN;
    first_guard->next = nullptr;
}

auto MemoryArena::check_integrity() const -> void {
    auto *guard = reinterpret_cast<ArenaGuard *>(memory);
    while (guard->next != nullptr) {
        if (guard->guard_pattern != GUARD_PATTERN) {
            log_error("MemoryArena: integrity check failed. Exiting...");
            exit(1);
        }
        guard = guard->next;
    }
}

auto MemoryArena::init(void *in_memory, u32 in_size) -> void {
    memory = static_cast<u8 *>(in_memory);
    size = in_size;
    clear();
}


void set_transient_arena(MemoryArena *arena) {
    assert(arena->memory != nullptr);
    transient = arena;
    clear_transient();
}

void clear_transient() {
    assert(transient != nullptr);
    assert(transient->size % 4 == 0);
    transient->used = 0;
    // TODO: Add if debug
    debug_set_memory(transient->memory, transient->size);
}

void *allocate_transient(u64 request_size) {
    assert(transient != nullptr);
    if ((transient->used + request_size) > transient->size) {
        log_info("Failed to allocate transient memory. The request was %d bytes, but only %d bytes remain.\n",
                 request_size, (transient->size - transient->used));
        exit(1);
    }
    void *result = transient->memory + transient->used;
    transient->used = request_size + transient->used;

    log_info("Allocated %llu / %llu bytes.", transient->used, transient->size);
    return result;
}