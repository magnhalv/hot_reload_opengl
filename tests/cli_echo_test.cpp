
#include "doctest.h"

#include <array.h>
#include <cli/echo.h>
#include <fixed_string.h>
#include "util.h"



TEST_CASE_FIXTURE(TransientFixture, "echo: too many arguments") {
    Array<FStr> args;
    args.init(local, 2);
    args[0] = FStr::create("Test", local);
    args[1] = FStr::create("Test2", local);
    LinkedListBuffer llb;
    llb.init(512, local);
    auto num_results = handle_echo(args, llb);
    REQUIRE_EQ(num_results, 1);
    REQUIRE_EQ(llb.list[0],  "Echo only accepts a single argument.");
}

TEST_CASE_FIXTURE(TransientFixture, "echo: happy path") {
    Array<FStr> args;
    args.init(local, 1);
    args[0] = FStr::create("Test", local);
    LinkedListBuffer llb;
    llb.init(512, local);
    auto num_results = handle_echo(args, llb);
    REQUIRE_EQ(num_results, 1);
    REQUIRE_EQ(llb.list[0], "Test");
}


