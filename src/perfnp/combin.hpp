// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef COMBIN_CONFIG_H_
#define COMBIN_CONFIG_H_

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

#include "perfnp/config.hpp"


namespace perfnp {

struct CommandLine {

    std::string m_command;

    std::vector<std::string> m_arguments;

    CommandLine() = default;

    CommandLine(
        std::string command,
        std::vector<std::string> arguments)
    : m_command(command)
    , m_arguments(arguments)
    {}

    bool operator== (const CommandLine& rhs) const {
        return m_command == rhs.m_command
            && m_arguments == rhs.m_arguments;
    }

}; // CommandLine

std::vector<CommandLine> combine_command_lines(const Config& config);

} // perfnp
#endif // COMBIN_CONFIG_H_
