// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/exec.hpp"

#include "catch.hpp"

using namespace perfnp;

TEST_CASE("ExecBin::ExecBin")
{
    ExecBin eb(std::string("foo"));
    REQUIRE(eb.binary() == "bar");
}
