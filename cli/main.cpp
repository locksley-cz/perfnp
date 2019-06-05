// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <perfnp/scheduler.hpp>

#include <cstdint>
#include <iostream>
#include <utility>
#include <fstream>

using namespace perfnp;

int main(int argc, char* argv[]) try {

    nlohmann::json config_json;
    if (argc <= 1) {
        config_json = nlohmann::json::parse(std::cin);
    } else {
        std::ifstream input(argv[1]);
        config_json = nlohmann::json::parse(input);
    }
    Config config(std::move(config_json));

    auto runs = combine_command_lines(config);
    std::cout << "Jobs to execute: " << runs.size() << std::endl;
    auto dataset = execute_all_runs(runs, config.timeout());

    std::cout << "Median runtime:  "
        << dataset.median_runtime_of_all_runs() << "s +- "
        << dataset.mad_runtime_of_all_runs() << "s" << std::endl;
    std::cout << "Successful runs: "
        << dataset.median_runtime_of_successful_runs() << "s +- "
        << dataset.mad_runtime_of_all_successful_runs() << "s" << std::endl;

} catch (const nlohmann::json::parse_error& ex) {
    std::cerr << "ERROR: Configuration file is not JSON." << std::endl;
    std::cerr << ex.what() << std::endl;
    return 1;

} catch(const std::runtime_error& ex) {
    std::cerr << "ERROR: " << ex.what() << std::endl;
    return 1;

} catch(...) {
    std::cerr << "ERROR: Unknown reason" << std::endl;
    return 1;
}
