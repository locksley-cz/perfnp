// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <perfnp/scheduler.hpp>
#include <perfnp/logger.hpp>

#include <cstdint>
#include <iostream>
#include <utility>
#include <fstream>

using namespace perfnp;

int main(int argc, char* argv[]) try {

    // Prepare the experiment

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

    // Open the CSV log file if needed

    auto csv_output_filename = config.logging_job_csv_file();
    std::ofstream csv_output_file;
    if (csv_output_filename == "-") {
        print_job_csv_header(std::cout);
    } else if (!csv_output_filename.is_empty()) {
        csv_output_file.open(*csv_output_filename);
        print_job_csv_header(csv_output_file);
    }

    // Run the experiment!

    auto dataset = execute_all_runs(runs, config.timeout(),
        [&](const std::vector<CommandLine>& commands,
            size_t job_index, unsigned timeout, ExecResult result)
        {
            if (csv_output_filename == "-") {
                print_job_csv_line(std::cout,
                    commands, job_index, timeout, result);
            } else if (csv_output_file.is_open()) {
                print_job_csv_line(csv_output_file,
                    commands, job_index, timeout, result);
            }
        });

    // Cleanup

    if (csv_output_file.is_open()) {
        csv_output_file.close();
    }

    // Print statistics

    if (dataset.number_of_all_successful_runs() > 0) {
        std::cout << "Median runtime:  "
            << dataset.median_runtime_of_all_runs() << "s +- "
            << dataset.mad_runtime_of_all_runs() << "s from "
            << runs.size() << " jobs" << std::endl;

        std::cout << "Successful runs: "
            << dataset.median_runtime_of_successful_runs() << "s +- "
            << dataset.mad_runtime_of_all_successful_runs() << "s from "
            << dataset.number_of_all_successful_runs() << " jobs" << std::endl;
    } else {
        std::cout << "There were no successful runs." << std::endl;
    }

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
