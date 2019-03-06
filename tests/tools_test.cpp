// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "catch.hpp"

#include <perfnp/tools.hpp>
using namespace perfnp::tools;

TEST_CASE("type_to_string::primitive_types")
{
    REQUIRE(type_to_string<void>() == "void");
    REQUIRE(type_to_string<bool>() == "bool");

    REQUIRE(type_to_string<float>() == "float");
    REQUIRE(type_to_string<double>() == "double");

    REQUIRE(type_to_string<char>() == "char");
    REQUIRE(type_to_string<short>() == "short");
    REQUIRE(type_to_string<int>() == "int");
    REQUIRE(type_to_string<long>() == "long");

    REQUIRE(type_to_string<unsigned char>() == "unsigned char");
    REQUIRE(type_to_string<unsigned short>() == "unsigned short");
    REQUIRE(type_to_string<unsigned int>() == "unsigned int");
    REQUIRE(type_to_string<unsigned long>() == "unsigned long");
}

class T {};
namespace n {
    class U {};
}

TEST_CASE("type_to_string::complex_types")
{
    REQUIRE(type_to_string<T>() == "T");
    REQUIRE(type_to_string<n::U>() == "n::U");
    REQUIRE(type_to_string<long>() == "long");
    REQUIRE(type_to_string<unsigned long>() == "unsigned long");
}
