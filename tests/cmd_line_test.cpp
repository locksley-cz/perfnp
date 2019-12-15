// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/cmd_line.hpp"

#include "catch.hpp"

using namespace perfnp;

TEST_CASE("CmdWithArgs::CmdWithArgs")
{
    CmdWithArgs cwa(4, "hello", {"world", "!"});
    REQUIRE(cwa.job_index() == 4);
    REQUIRE(cwa.command() == "hello");
    REQUIRE(cwa.arguments() == std::vector<std::string>({"world", "!"}));
}



TEST_CASE("CmdWithArgs::escape_for_bash")
{
    SECTION("simple strings")
    {
        CmdWithArgs cwa(0, "hello", {"world"});
        std::string out = cwa.escape_for_bash();
        REQUIRE(out == "hello world");
    }

    SECTION("quoting gets activated")
    {
        SECTION("argument needs quoting")
        {
            CmdWithArgs cwa(4, "hello", {"wor ld"});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "hello 'wor ld'");
        }

        SECTION("command needs quoting")
        {
            CmdWithArgs cwa(4, "hel lo", {"world"});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel lo' world");
        }
    }

    SECTION("different quoting characters")
    {
        SECTION("space")
        {
            CmdWithArgs cwa(4, "hel lo", {});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel lo'");
        }

        SECTION("dollar")
        {
            CmdWithArgs cwa(4, "hel$lo", {});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel$lo'");
        }

        SECTION("star")
        {
            CmdWithArgs cwa(4, "hel*lo", {});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel*lo'");
        }

        SECTION("curly brackets")
        {
            CmdWithArgs cwa(4, "hel{}lo", {});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel{}lo'");
        }

        SECTION("new line")
        {
            CmdWithArgs cwa(4, "hel\nlo", {});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel\nlo'");
        }

        SECTION("tab")
        {
            CmdWithArgs cwa(4, "hel\tlo", {});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel\tlo'");
        }

        SECTION("cr")
        {
            CmdWithArgs cwa(4, "hel\rlo", {});
            std::string out = cwa.escape_for_bash();
            REQUIRE(out == "'hel\rlo'");
        }
    }
} // escape_for_bash



TEST_CASE("CmdWithArgs::escape_for_command_prompt")
{
    SECTION("simple strings")
    {
        CmdWithArgs cwa(0, "hello", {"world"});
        std::wstring out = cwa.escape_for_cmd_exe();
        REQUIRE(out == L"hello world");
    }

    SECTION("quoting gets activated")
    {
        SECTION("argument needs quoting")
        {
            CmdWithArgs cwa(4, "hello", {"wor ld"});
            std::wstring out = cwa.escape_for_cmd_exe();
            REQUIRE(out == std::wstring(L"hello \"wor ld\""));
        }

        SECTION("command needs quoting")
        {
            CmdWithArgs cwa(4, "hel lo", {"world"});
            std::wstring out = cwa.escape_for_cmd_exe();
            REQUIRE(out == std::wstring(L"\"hel lo\" world"));
        }
    }

    SECTION("different quoting characters")
    {
        SECTION("space")
        {
            CmdWithArgs cwa(4, "hel lo", {});
            std::wstring out = cwa.escape_for_cmd_exe();
            REQUIRE(out == std::wstring(L"\"hel lo\""));
        }

        SECTION("tab")
        {
            CmdWithArgs cwa(4, "hel\tlo", {});
            std::wstring out = cwa.escape_for_cmd_exe();
            REQUIRE(out == std::wstring(L"\"hel\tlo\""));
        }

        SECTION("vertical tab")
        {
            CmdWithArgs cwa(4, "hel\vlo", {});
            std::wstring out = cwa.escape_for_cmd_exe();
            REQUIRE(out == std::wstring(L"\"hel\vlo\""));
        }

        SECTION("new line")
        {
            CmdWithArgs cwa(4, "hel\nlo", {});
            std::wstring out = cwa.escape_for_cmd_exe();
            REQUIRE(out == std::wstring(L"\"hel\nlo\""));
        }

        SECTION("quotation")
        {
            CmdWithArgs cwa(4, "hel\"lo", {});
            std::wstring out = cwa.escape_for_cmd_exe();
            REQUIRE(out == std::wstring(L"\"hel\\\"lo\""));
        }
    }
} // escape_for_command_prompt
