// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/logger.hpp"
#include "perfnp/tools.hpp"

#include <iostream>
#include <iomanip>
#include <vector>

using namespace perfnp;

void perfnp::print_job_csv_header(std::ostream& o)
{
    // Make sure to revert all stdw and similar
    tools::StreamFormatGuard sfg(o);

    o << std::setw(10) << "Job ID" << ";";
    o << std::setw(10) << "Runtime" << ";";
    o << std::setw(10) << "Exit code" << ";";

    o << std::setw(0);
    o << " Command" << std::endl;
} // print_csv_header



void perfnp::print_job_csv_line(std::ostream& o,
    const std::vector<CommandLine>& commands,
    size_t job_index,
    unsigned timeout,
    ExecResult result)
{
    // Make sure to revert all stdw and similar
    tools::StreamFormatGuard sfg(o);

    auto& command = commands.at(job_index);

    o << std::setw(10) << job_index << ";";
    o << std::setw(10) << result.runtime() << ";";
    o << std::setw(10) << result.exit_code() << ";";

    o << std::setw(0) << " ";
    o << command.m_command;
    for (const auto& arg : command.m_arguments) {
        o << " \"" << arg << "\"";
    }
    o << std::endl;
} // print_csv_line
