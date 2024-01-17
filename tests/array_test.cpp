#include "doctest.h"

#include <array.h>
#include <memory_arena.h>
#include "util.h"

TEST_CASE("test") {

    void *data = malloc(sizeof (u8)*1024);
    MemoryArena arena;
    arena.init(data, 1024);


    Array<int> arr;
    CHECK_CRASH(arr.init(arena, 1024) , "Failed to allocate 4096 bytes. Only 0 remaining.");
}
