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
template<typename ResultCallback>
Dataset execute_all_runs(
    const std::vector<CmdWithArgs>& commands,
    unsigned timeout,
    ResultCallback callback)
{
    std::vector<ExecResult> results_all;

    for (size_t i=0; i < commands.size(); i++) {
        const auto& cwa = commands.at(i);

        ExecBin my_exec(cwa.command(), cwa.arguments(), timeout);
        ExecResult my_result = my_exec.execute();

        callback(cwa, timeout, my_result);

        results_all.push_back(my_result);
    }

    return Dataset(timeout, results_all);
} // execute_all_runs



/*!
 * Executes all commands and creates a dataset out of the results.
 */
Dataset execute_all_runs(
    const std::vector<CmdWithArgs>& commands,
    const Config& config,
    unsigned timeout
) {
    return execute_all_runs<>(commands, timeout,
        [](const CmdWithArgs&, unsigned, ExecResult){}
    );
}

} // perfnp
#endif // PERFNP_STAT_H_
