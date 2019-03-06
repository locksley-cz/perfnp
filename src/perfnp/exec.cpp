// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/exec.hpp"

#include <cassert>

using namespace perfnp;

ExecBin::ExecBin(
    const std::string& binary,
    const std::vector<std::string>& args)
: m_binary(binary)
, m_args(args)
{
    assert(!binary.empty() && "Name of the executable must not be empty");
}