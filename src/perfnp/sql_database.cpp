#include <cstdio>
#include <stdint.h>
#include "perfnp/sql_database.hpp"
#include "perfnp/tools.hpp"
#include <string>
#include "base64.h"
#include <iostream>
#include<fstream>
#include<sstream>

#include <iomanip>
#include <vector>
#include<algorithm>
#include <unordered_set>

using namespace perfnp;
using namespace std;

namespace perfnp {


sql_database::sql_database(const std::string& database_filename)
: m_db(database_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    if (!m_db.tableExists("run")) {
        m_db.exec("CREATE TABLE run ("
            "run_id INTEGER PRIMARY KEY, "
            "started DATETIME)"
        );
    }

    if (!m_db.tableExists("job")) {
        m_db.exec("CREATE TABLE job ("
            "job_id INTEGER PRIMARY KEY, "
            "run_id INTEGER NOT NULL, "
            "job_index INTEGER NOT NULL, "
            "timeout INTEGER NOT NULL, "
            "exit_code INTEGER NOT NULL, "
            "runtime INTEGER NOT NULL,"
            "FOREIGN KEY (run_id) REFERENCES run(run_id))"
        );
    }

    if (!m_db.tableExists("command")) {
        m_db.exec("CREATE TABLE command ("
            "job_id INTEGER NOT NULL UNIQUE, "
            "commands TEXT NOT NULL, "
            "FOREIGN KEY(job_id) REFERENCES job(job_id))"
        );
    }

    if (!m_db.tableExists("image")) {
        m_db.exec("CREATE TABLE image ("
            "run_id INTEGER NOT NULL UNIQUE, "
            "config_file TEXT NOT NULL, "
            "command_file TEXT NOT NULL, "
            "FOREIGN KEY(run_id) REFERENCES run(run_id))"
        );
    }
}



long long sql_database::new_run_started() {
    m_db.exec(std::string("INSERT INTO run "
        "(run_id, started) VALUES "
        "(NULL, datetime('now','localtime'))"));
    return m_db.getLastInsertRowid();
}



std::time_t sql_database::get_time_run_started(long long run_id)
{

    SQLite::Statement query(m_db, "SELECT started FROM run WHERE run_id = ?");
    query.bind(1, run_id);

    if (!query.executeStep()) {
        throw runtime_error("Requested run is not present in the database");
    }

    std::tm parsed_date_time;
    {
        std::stringstream stream(query.getColumn("started").getString());
        stream >> std::get_time(&parsed_date_time, "%Y-%m-%d %H:%M:%S");
    }

    std::size_t out = std::mktime(&parsed_date_time);
    if (out < 0) {
        throw runtime_error("Started time in the 'run' table has a wrong format.");
    }

    if (query.executeStep()) {
        throw runtime_error("Primary key of the 'run' table is violated.");
    }

    return out;
}



namespace {

    //! Reads an entier file to a string
    std::string read_file_to_string(const std::string& filename)
    {
        std::ifstream file(filename); //taking file as inputstream
        if (file) {
            ostringstream ss;
            ss << file.rdbuf();
            return ss.str();
        } else {
            return "";
        }
    } // read_file_to_string



    //! Converts a string to UTF-8
    std::string convert_to_base64(std::string data)
    {
        auto ptr = reinterpret_cast<const unsigned char*>(data.c_str());
        assert(data.length() < static_cast<size_t>(std::numeric_limits<unsigned int>::max()));
        std::string encoded = base64_encode(ptr, static_cast<unsigned>(data.length()));
        assert(base64_decode(encoded) == data);
        return encoded;
    } // convert_to_base64

} // anonymous namespace



void sql_database::save_config_and_command_read_from_file(
    long long run_id, const Config& config)
{
    save_config_and_command_given_directly(run_id, config,
        read_file_to_string(config.command())
    );
}



void sql_database::save_config_and_command_given_directly(long long run_id,
    const Config& config, const std::string& command_content)
{
    std::string config_base64 = convert_to_base64(config.to_string());
    std::string command_base64 = convert_to_base64(command_content);

    SQLite::Statement commands_stmt(m_db, "INSERT INTO image VALUES (?,?,?)");
    commands_stmt.bind(1, run_id);
    commands_stmt.bind(2, config_base64);
    commands_stmt.bind(3, command_base64);
    commands_stmt.exec();
}



void sql_database::remove_finished_jobs(
    std::vector<CmdWithArgs>& jobs,
    const Config& config)
{
    std::unordered_set<unsigned> already_finished;

    SQLite::Statement query(m_db,
        "SELECT job_index"
        " FROM job WHERE run_id = ("
            " SELECT MAX(run.run_id)"
            " FROM run JOIN job ON job.run_id = run.run_id"
        ")"
    );

    while (query.executeStep()) {
        already_finished.insert(query.getColumn("job_index").getUInt());
    }

    jobs.erase(std::remove_if(jobs.begin(), jobs.end(), [&](const CmdWithArgs& job) {
        return already_finished.find(job.job_index()) != already_finished.end();
    }), jobs.end());
}

long long sql_database::on_job_finished(long long run_id,
    const CmdWithArgs& cwa, unsigned timeout, ExecResult result)
{
    // 1) Insert job
    std::string output_jobs_info = std::string("INSERT INTO job VALUES (")
        + "NULL,"
        + std::to_string(run_id) + ", "
        + std::to_string(cwa.job_index()) + ", "
        + std::to_string(timeout) + ", "
        + std::to_string(result.exit_code()) + ", "
        + std::to_string(result.runtime()) + ")";

    m_db.exec(output_jobs_info);
    long long run_primary_key = m_db.getLastInsertRowid();

    // 2) Insert command ID
    SQLite::Statement commands_stmt(m_db, "INSERT INTO command VALUES (?,?)");
    commands_stmt.bind(1, run_primary_key);
    commands_stmt.bind(2, cwa.escape_for_native_shell());
    commands_stmt.exec();

    return run_primary_key;
} // on_job_finished



void sql_database::read()
{
    SQLite::Statement m_query1(m_db, "SELECT * FROM `jobs_info`");
    m_query1.executeStep();

    int exit = m_query1.getColumn("exit_code").getInt();
    int time = m_query1.getColumn("runtime").getInt();
    int timeout = m_query1.getColumn("timeout").getInt();

    //std::cout << exit <<std::endl<<time<<std::endl<<timeout<<std::endl;
}

} // perfnp namespace
