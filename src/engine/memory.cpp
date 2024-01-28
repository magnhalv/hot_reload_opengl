#include <cassert>
#include "memory.h"

void debug_set_memory(void *memory, u64 size) {
    assert(size % 4 == 0);
    u8 *mem = reinterpret_cast<u8*>(memory);
    for (u64 i = 0; i < size; i++) {
        mem[i] = 0; // NOTE: Little-endian representation of 0xDEADBEEF
    }
}
