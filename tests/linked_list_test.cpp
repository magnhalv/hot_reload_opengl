#include "util.h"
#include <linked_list.h>
#include <fixed_string.h>

TEST_CASE_FIXTURE(SingleArenaFixture, "basic case with fixed strings") {
    LinkedList<FixedString> list;
    FixedString test_strings[2] = {
            FixedString::create("test", arena),
            FixedString::create("test2", arena)
    };
    list.insert(test_strings[0], arena);
    list.insert(test_strings[1], arena);

    REQUIRE_EQ(list.size(), 2);

    int index = 0;
    for (auto entry : list) {
        REQUIRE_EQ(entry, test_strings[index]);
        index++;
    }
}