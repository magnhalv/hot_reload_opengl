#include "memory.h"

void debug_set_memory(void *memory, u64 size) {
    u32 *mem = reinterpret_cast<u32*>(memory);
    for (int i = 0; i < size/4; i++) {
        mem[i] = 0xEFBEADDE; // NOTE: Little-endian representation of 0xDEADBEEF
    }
}
