#include "perfnp/scheduler.hpp"

using namespace perfnp;

Dataset perfnp::execute_all_runs(
    const std::vector<CommandLine>& commands,
    unsigned timeout) {

    std::vector<ExecResult> results_all;
    for (size_t i=0; i<commands.size(); i++) {
        ExecBin my_exec(
            commands.at(i).m_command,
            commands.at(i).m_arguments, timeout);
        ExecResult my_result = my_exec.execute();
        results_all.push_back(my_result);
    }

    return Dataset(timeout, results_all);
}
