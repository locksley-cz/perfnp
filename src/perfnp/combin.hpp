// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef COMBIN_CONFIG_H_
#define COMBIN_CONFIG_H_

//#include <nlohmann/json.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <ostream>

#include "perfnp/cmd_line.hpp"
#include "perfnp/config.hpp"

namespace perfnp {

std::vector<CmdWithArgs> combine_command_lines(const Config& config);

} // perfnp

#endif // COMBIN_CONFIG_H_
