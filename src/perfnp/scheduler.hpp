// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_SCHEDULER_H_
#define PERFNP_SCHEDULER_H_

#include "perfnp/combin.hpp"
#include "perfnp/dataset.hpp"
#include "perfnp/config.hpp"
#include "perfnp/exec.hpp"

#include <string>
#include <vector>
#include <iostream>

namespace perfnp {

/*!
 * Executes all commands and creates a dataset out of the results.
 */
Dataset execute_all_runs(
    const std::vector<CommandLine>& commands,
    unsigned timeout
);
} // perfnp
#endif // PERFNP_STAT_H_
