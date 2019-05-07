// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/config.hpp"

#include "catch.hpp"

using namespace perfnp;
using namespace std;

using json = nlohmann::json;

TEST_CASE("Config::timeout")
{
    SECTION("standard operation")
    {
        Config c(R"({ "timeout" : 10 })"_json);
        REQUIRE(c.timeout() == 10);
    }

    SECTION("field is missing")
    {
        Config c(R"({})"_json);
        REQUIRE_THROWS_AS(c.timeout(), std::runtime_error);
    }

    SECTION("field has invalid type")
    {
        Config c(R"({"timeout":"hello"})"_json);
        REQUIRE_THROWS_AS(c.timeout(), std::runtime_error);
    }
}

TEST_CASE("Config::command")
{
    SECTION("standard operation")
    {
        Config c(R"({"command":"golem" })"_json);
        REQUIRE(c.command() == std::string("golem"));
    }

    SECTION("field is missing")
    {
        Config c(R"({})"_json);
        REQUIRE_THROWS_AS(c.command(), std::runtime_error);
    }

    SECTION("field has invalid type")
    {
        Config c(R"({"command":0})"_json);
        REQUIRE_THROWS_AS(c.command(), std::runtime_error);
    }
}

TEST_CASE("Config::arguments")
{
    SECTION("positive cases")
    {
        SECTION("only one argument")
        {
            Config c(R"({ "arguments" : ["one"]})"_json);
            Arguments expected({"one"});
            REQUIRE(c.arguments() == expected);
        }

        SECTION("three arguments")
        {
            Config c(R"({"arguments":["$blockvar", "$parameter", "$file"] })"_json);
            Arguments expected({"$blockvar", "$parameter", "$file"});
            REQUIRE(c.arguments() == expected);
        }
    }

    SECTION("corner cases")
    {
        SECTION("no arguments")
        {
            Config c(R"({ "arguments" : []})"_json);
            Arguments expected;
            REQUIRE(c.arguments() == expected);
        }
    }

    SECTION("negative cases")
    {
        SECTION("missing field")
        {
            Config c(R"({ })"_json);
            REQUIRE_THROWS_AS(c.arguments(), std::runtime_error);
        }

        SECTION("wrong outer type")
        {
            Config c(R"({ "arguments" : {} })"_json);
            REQUIRE_THROWS_AS(c.arguments(), std::runtime_error);
        }

        SECTION("wrong inner type")
        {
            Config c(R"({ "arguments" : [{}] })"_json);
            REQUIRE_THROWS_AS(c.arguments(), std::runtime_error);
        }
    }
}

TEST_CASE("Config::parameters")
{
    SECTION("positive cases")
    {
        SECTION("one parameter with values")
        {
            Config c(R"({"parameters":{
                "param_name" : ["blockvar", "parameter", "file"]
            }})"_json);

            Parameter expected2("param_name",
                std::vector<std::string>{
                    "blockvar", "parameter", "file"
                } );
            REQUIRE(c.parameters() == std::vector<Parameter>{expected2});
        }
    }

    SECTION("corner cases")
    {
        SECTION("no parameters at all")
        {
            Config c(R"({ "parameters":{} })"_json);
            REQUIRE_THROWS_AS(c.parameters(), std::runtime_error);
        }
    }

    SECTION("negative cases")
    {
        SECTION("empty name")
        {
            Config c(R"({"parameters":{"":[]} })"_json);
            REQUIRE_THROWS_AS(c.parameters(), std::runtime_error);
        }

        SECTION("empty set of value")
        {
            Config c(R"({"parameters":{"v":[]} })"_json);
            REQUIRE_THROWS_AS(c.parameters(), std::runtime_error);
        }

        SECTION("invalid type of the map")
        {
            Config c(R"({ "parameters":[] })"_json);
            REQUIRE_THROWS_AS(c.parameters(), std::runtime_error);
        }

        SECTION("invalid type of the values")
        {
            Config c(R"({ "parameters":{ "v":2 } })"_json);
            REQUIRE_THROWS_AS(c.parameters(), std::runtime_error);
        }

        SECTION("invalid type of a value")
        {
            Config c(R"({ "parameters":{ "v":[2] } })"_json);
            REQUIRE_THROWS_AS(c.parameters(), std::runtime_error);
        }
    }
}
