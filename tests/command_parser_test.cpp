
#include "doctest.h"

#include <array.h>
#include <memory_arena.h>
#include "command_parser.h"
#include <fixed_string.h>
#include "util.h"
#include "linked_list.h"


/*
TEST_CASE_FIXTURE(TransientFixture, "basic case") {
    auto &graphics = create_cli_app("graphics", local);
    auto &anti_alias = add_subcommand(graphics, "anti-alias", local);
    bool enable = false;
    bool disable = false;
    add_flag(anti_alias, "e", "enable", &enable, local);
    add_flag(anti_alias, "d", "disable", &disable, local);


    FixedString text = FixedString::create("graphics anti-alias -e", local);
    LinkedList<FixedString> parsed_input = parse_command(text, graphics, local);

    REQUIRE(enable);
}
*/

