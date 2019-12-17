// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <perfnp/scheduler.hpp>
#include <perfnp/logger.hpp>
#include <perfnp/config.hpp>
#include <perfnp/sql_database.hpp>
#include <perfnp/dataset.hpp>
#include <cstdint>
#include <iostream>
#include <utility>
#include <fstream>
#include <string>
#include <vector>

using namespace perfnp;

int main(int argc, char* argv[]) try {

    // Prepare the experiment

    bool resume = false;
    std::string first_parameter(argv[1]);
    nlohmann::json config_json;
    if (argc <= 1) {
        config_json = nlohmann::json::parse(std::cin);
    } else if (argc == 2 && first_parameter != "-r") {
        std::ifstream input(argv[1]);
        config_json = nlohmann::json::parse(input);
    } else if (argc == 3) {
        if (first_parameter == "-r") {
            resume = true;
            std::cout << "Resume ON!" << std::endl;
        }
        std::ifstream input(argv[2]);
        config_json = nlohmann::json::parse(input);
    }
    Config config(std::move(config_json));


    auto jobs = combine_command_lines(config);
    std::cout << "Jobs to execute: " << jobs.size() << std::endl;

    // Open the CSV log file if needed

    auto csv_output_filename = config.logging_job_csv_file();
    std::ofstream csv_output_file;
    if (csv_output_filename == "-") {
        print_job_csv_header(std::cout);
    } else if (csv_output_filename.is_empty()) {
        csv_output_file.open(*csv_output_filename);
        print_job_csv_header(csv_output_file);
    }

    sql_database db("perfnp.sqlite");
    auto run_id = db.new_run_started();
    db.save_config_and_command_read_from_file(run_id, config);

    if (resume) {
        db.remove_finished_jobs(jobs, config);
    }

    // Run the experiment!
    auto dataset = execute_all_runs(jobs, config.timeout(),
        [&](const CmdWithArgs& cwa, unsigned timeout, ExecResult result)
        {
            std::cerr << "Job " << cwa.job_index() << " has finished." << std::endl;
            if (csv_output_filename == "-") {
                print_job_csv_line(std::cout, cwa, timeout, result);
            } else if (csv_output_file.is_open()) {
                print_job_csv_line(csv_output_file, cwa, timeout, result);
            }

            std::cerr << "Storing the record in the database." << std::endl;
            db.on_job_finished(run_id, cwa, timeout, result);
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
            << jobs.size() << " jobs" << std::endl;

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
