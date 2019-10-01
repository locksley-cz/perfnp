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
            "parameters" : {
                "a" : ["1", "2"],
                "b" : ["x", "y"]
            }
        })"_json);

        CommandLine exp1("hello", {"world", "1", "x"});
        CommandLine exp2("hello", {"world", "1", "y"});
        CommandLine exp3("hello", {"world", "2", "x"});
        CommandLine exp4("hello", {"world", "2", "y"});

        REQUIRE_THAT(combine_command_lines(c), Catch::Matchers::UnorderedEquals(
            std::vector<CommandLine>{exp1, exp2, exp3, exp4}
        ));
    }

    SECTION("more complex unit test")
    {
        Config c(R"({
            "command" : "hello",
            "arguments" : ["%c%", "world", "%a%", "%b%"],
            "parameters" : {
                "a" : ["1", "2"],
                "b" : "1",
                "c" : ["x", "y", "z"]
            }
        })"_json);

        CommandLine exp1("hello", {"x", "world", "1", "1"});
        CommandLine exp2("hello", {"x", "world", "2", "1"});
        CommandLine exp3("hello", {"y", "world", "1", "1"});
        CommandLine exp4("hello", {"y", "world", "2", "1"});
        CommandLine exp5("hello", {"z", "world", "1", "1"});
        CommandLine exp6("hello", {"z", "world", "2", "1"});
        REQUIRE_THAT(combine_command_lines(c), Catch::Matchers::UnorderedEquals(
            std::vector<CommandLine>{exp1, exp2, exp3, exp4, exp5, exp6}
        ));
    }

    SECTION("only one parameter")
    {
        Config c(R"({
            "command" : "hello",
            "arguments" : ["%x%"],
            "parameters" : {
                "x" : "1"
            }
        })"_json);

        CommandLine exp1("hello", {"1"});
        REQUIRE(combine_command_lines(c) ==
            std::vector<CommandLine>{exp1}
        );
    }
}
