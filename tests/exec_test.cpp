// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/exec.hpp"

#include "catch.hpp"

#include <chrono>
#include <iostream>

using namespace perfnp;

TEST_CASE("ExecBin::execute")
{
#if defined(_WIN32)
    SECTION("The binary is executed")
    {
        ExecBin eb("TIMEOUT", { "1" });
        auto result = eb.execute();
        REQUIRE(result.exit_code() == 0);
        // Windows TIMEOUT command with
        // argument N waits _at most_ N
        // seconds + some OS overhead.
        REQUIRE(1 <= result.runtime());
        REQUIRE(result.runtime() <= 2);
    }

    SECTION("Binary with many arguments")
    {
        ExecBin eb("PING", { "127.0.0.1", "-n", "2" });
        auto result = eb.execute();
        REQUIRE(result.exit_code() == 0);
        REQUIRE(result.runtime() == 2);
    }
#elif defined(__linux__) || defined(__APPLE__)
    SECTION("The binary is executed")
    {
        ExecBin eb("sleep", { "1" });
        auto result = eb.execute();
        REQUIRE(result.exit_code() == 0);
        REQUIRE(result.runtime() == 2);
    }
#endif
}



TEST_CASE("ExecBin::timeout")
{
    SECTION("Binary is killed before the timeout")
    {
#if defined(_WIN32)
        ExecBin eb("TIMEOUT", { "3" }, 1);
#elif defined(__linux__) || defined(__APPLE__)
        ExecBin eb("sleep", { "2" }, 1);
#endif
        auto start_time = std::chrono::steady_clock::now();
        auto result = eb.execute();
        auto end_time = std::chrono::steady_clock::now();

        REQUIRE(result.exit_code() != 0);
        REQUIRE(result.runtime() <= 2);
        REQUIRE(std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() < 2);
    }
}



TEST_CASE("ExecBin::error_handling")
{
    SECTION("Empty binary detected") {
        REQUIRE_THROWS_AS(ExecBin("", {}, 1), std::runtime_error);
    }

    SECTION("Exec-result doesn't accept 0 runtime") {
        REQUIRE_THROWS_AS(ExecResult(1, 0), std::runtime_error);
    }
}
