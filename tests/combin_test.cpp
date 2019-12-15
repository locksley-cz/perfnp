// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/combin.hpp"
#include "perfnp/config.hpp"
#include "catch.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
// for convenience
using json = nlohmann::json;

using namespace perfnp;
using namespace std;

TEST_CASE("combine_command_lines")
{
    SECTION("two parameters with values")
    {
        Config c(R"({
            "command" : "hello",
            "arguments" : ["world", "%a%", "%b%"],
            "parameters" : [
                { "name" : "a", "values" : ["1", "2"] },
                { "name" : "b", "values" : ["x", "y"] }
            ]
        })"_json);

        CmdWithArgs exp1(0, "hello", {"world", "1", "x"});
        CmdWithArgs exp2(1, "hello", {"world", "1", "y"});
        CmdWithArgs exp3(2, "hello", {"world", "2", "x"});
        CmdWithArgs exp4(3, "hello", {"world", "2", "y"});

        REQUIRE_THAT(combine_command_lines(c), Catch::Matchers::UnorderedEquals(
            std::vector<CmdWithArgs>{exp1, exp2, exp3, exp4}
        ));
    }

    SECTION("more complex unit test")
    {
        Config c(R"({
            "command" : "hello",
            "arguments" : ["%c%", "world", "%a%", "%b%"],
            "parameters" : [
                { "name" : "a", "values" : ["1", "2"] },
                { "name" : "b", "values" : ["1"] },
                { "name" : "c", "values" : ["x", "y", "z"] }
            ]
        })"_json);

        CmdWithArgs exp1(0, "hello", {"x", "world", "1", "1"});
        CmdWithArgs exp3(1, "hello", {"y", "world", "1", "1"});
        CmdWithArgs exp5(2, "hello", {"z", "world", "1", "1"});
        CmdWithArgs exp2(3, "hello", {"x", "world", "2", "1"});
        CmdWithArgs exp4(4, "hello", {"y", "world", "2", "1"});
        CmdWithArgs exp6(5, "hello", {"z", "world", "2", "1"});
        REQUIRE_THAT(combine_command_lines(c), Catch::Matchers::UnorderedEquals(
            std::vector<CmdWithArgs>{exp1, exp2, exp3, exp4, exp5, exp6}
        ));
    }
}
