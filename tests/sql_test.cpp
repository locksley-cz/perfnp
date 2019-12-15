// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/sql_database.hpp"

#include "catch.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <chrono>

using namespace perfnp;

const std::string TEST_DATABASE_FILENAME("temporary_database_for_tests.sqlite");


TEST_CASE("sql_database::sql_database")
{
    SECTION("database can be created")
    {
        sql_database db(TEST_DATABASE_FILENAME);
    }

    SECTION("database can be created and reopened")
    {
        {
            sql_database db(TEST_DATABASE_FILENAME);
        }
        { // Here, we are 100% reopneing an existing database.
            sql_database db(TEST_DATABASE_FILENAME);
        }
    }

    std::remove(TEST_DATABASE_FILENAME.c_str());
}



TEST_CASE("sql_database::new_run_started")
{
    SECTION("two runs will receive two different ids")
    {
        sql_database db(TEST_DATABASE_FILENAME);
        auto id1 = db.new_run_started();
        auto id2 = db.new_run_started();
        REQUIRE(id1 != id2);
    }

    SECTION("time is properly saved and loaded")
    {
        sql_database db(TEST_DATABASE_FILENAME);
        auto start = std::chrono::system_clock::now();
        auto run_id = db.new_run_started();
        auto end = std::chrono::system_clock::now();
    }

    std::remove(TEST_DATABASE_FILENAME.c_str());
}

TEST_CASE("sql_database::remove_finished_jobs")
{
    Config c(R"({
        "command" : "sleep",
        "arguments" : ["%time%"],
        "parameters" : {
            "time" : ["1", "2", "3"]
        }
    })"_json);

    CmdWithArgs exp1(0, "sleep", {"1"});
    CmdWithArgs exp2(1, "sleep", {"2"});
    CmdWithArgs exp3(2, "sleep", {"3"});

    SECTION("if no jobs are in the database") {
        sql_database ts_db(TEST_DATABASE_FILENAME);

        auto run_id = ts_db.new_run_started();
        SECTION("and we remove finished jobs")
        {
            std::vector<CmdWithArgs> example{exp1, exp2, exp3};
            ts_db.remove_finished_jobs(example,c);
            SECTION("all jobs remain there")
            {
                REQUIRE(example == std::vector<CmdWithArgs>{exp1, exp2, exp3});
            }
        }
    }

    SECTION("if jobs are saved to the database") {
        sql_database ts_db(TEST_DATABASE_FILENAME);

        auto run_id = ts_db.new_run_started();
        ts_db.on_job_finished(run_id, exp1, 10, ExecResult(0, 1));
        ts_db.on_job_finished(run_id, exp2, 10, ExecResult(0, 2));
        ts_db.on_job_finished(run_id, exp3, 10, ExecResult(0, 3));

        SECTION("and we remove finished jobs") {
            std::vector<CmdWithArgs> example{exp1, exp2, exp3};
            ts_db.remove_finished_jobs(example,c);

            SECTION("there are no jobs left") {
                REQUIRE(example.empty());
            }
        }
    }

    SECTION("if not all jobs are saved to the database") {
        sql_database ts_db(TEST_DATABASE_FILENAME);

        auto run_id = ts_db.new_run_started();
        ts_db.on_job_finished(run_id, exp1, 10, ExecResult(0, 1));
        ts_db.on_job_finished(run_id, exp3, 10, ExecResult(0, 3));

        SECTION("and we remove finished jobs") {
            std::vector<CmdWithArgs> example{exp1, exp2, exp3};
            ts_db.remove_finished_jobs(example,c);

            SECTION("the remaining one is still in the vector")
            {
                REQUIRE(example == std::vector<CmdWithArgs>{exp2});
            }
        }
    }

    SECTION("if more runs exist")
    {
        sql_database ts_db(TEST_DATABASE_FILENAME);

        auto run_id = ts_db.new_run_started();
        ts_db.on_job_finished(run_id, exp2, 10, ExecResult(0, 2));

        run_id = ts_db.new_run_started();
        ts_db.on_job_finished(run_id, exp1, 10, ExecResult(0, 1));
        ts_db.on_job_finished(run_id, exp2, 10, ExecResult(0, 2));
        ts_db.on_job_finished(run_id, exp3, 10, ExecResult(0, 3));

        SECTION("only the last runs is used") {
            std::vector<CmdWithArgs> example{exp1, exp2, exp3};
            ts_db.remove_finished_jobs(example,c);

            SECTION("and there are no jobs left") {
                REQUIRE(example.empty());
            }
        }
    }

    SECTION("if there is a run without any job")
    {
        sql_database ts_db(TEST_DATABASE_FILENAME);

        auto run_id = ts_db.new_run_started();
        ts_db.on_job_finished(run_id, exp1, 10, ExecResult(0, 1));
        ts_db.on_job_finished(run_id, exp2, 10, ExecResult(0, 2));
        ts_db.on_job_finished(run_id, exp3, 10, ExecResult(0, 3));

        ts_db.new_run_started();

        SECTION("than the previous run is considered")
        {
            std::vector<CmdWithArgs> example{exp1, exp2, exp3};
            ts_db.remove_finished_jobs(example,c);
            REQUIRE(example.empty());
        }
    }
}
