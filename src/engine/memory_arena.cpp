#include <cassert>
#include <cstdlib>

#include "memory_arena.h"
#include "memory.h"

MemoryArena *transient = nullptr;

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

void* allocate_transient(u64 request_size) {
    assert(transient != nullptr);
    if ((transient->used + request_size) > transient->size) {
        log_info("Failed to allocate transient memory. The request was %d bytes, but only %d bytes remain.\n", request_size, (transient->size - transient->used));
        exit(1);
    }
    void* result = transient->memory + transient->used;
    transient->used = request_size + transient->used;

    log_info("Allocated %llu / %llu bytes.", transient->used, transient->size);
    return result;
}