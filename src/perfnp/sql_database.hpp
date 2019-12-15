#ifndef PERFNP_SQL_DATABASE_H_
#define PERFNP_SQL_DATABASE_H_

#include "perfnp/combin.hpp"
#include "perfnp/exec.hpp"
#include "perfnp/config.hpp"

#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>

#include <ctime>
#include <string>
#include <iostream>
#include <fstream>


namespace perfnp {

class sql_database {

    SQLite::Database m_db;

public:
    //! Create a new database
    sql_database(const std::string& database_filename);

    long long new_run_started();

    std::time_t get_time_run_started(long long run_id);

    void save_config_and_command_read_from_file(
        long long run_id, const Config& config);

    void save_config_and_command_given_directly(long long run_id,
        const Config& config, const std::string& command_content);

    /*!
     * Removes such jobs from a vector that have already been saved in a previous run.
     *
     * First, this methods determines if there has been a run with the given
     * configuration. If not, the vector of jobs is kept intact. If yes, it
     * removes those jobs from the vector, which have finished in the past run.
     *
     * The identity of a job is determined by its index.
     * See \ref CmdWithArgs.run_index().
     */
    void remove_finished_jobs(std::vector<CmdWithArgs>& jobs, const Config& config);

    long long on_job_finished(long long run_id,
        const CmdWithArgs& cwa, unsigned timeout, ExecResult result);





private:

    void read();

}; // sql_database
} // perfnp
#endif // PERFNP_CORE_H_
