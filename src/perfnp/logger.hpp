// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_LOGGER_H_
#define PERFNP_LOGGER_H_

#include "perfnp/combin.hpp"
#include "perfnp/exec.hpp"
#include "perfnp/config.hpp"
namespace perfnp {

//! Prints the CSV header for \link print_job_csv_line format.
void print_job_csv_header(std::ostream& o);

//! Prints one CSV line for every executed job.
void print_job_csv_line(std::ostream& o,
    const CmdWithArgs& command, unsigned timeout, ExecResult result);

} // perfnp
#endif // PERFNP_CORE_H_
