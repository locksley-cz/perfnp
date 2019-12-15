// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/dataset.hpp"
#include <nlohmann/json.hpp>
#include "catch.hpp"

using namespace perfnp;

TEST_CASE("Dataset::median_runtime_of_all_runs")
{
    SECTION("Typical usage")
    {
        SECTION("Sorted input") {
            Dataset d(10, { {0,3}, {0,4}, {0,5} });
            REQUIRE(d.median_runtime_of_all_runs() == 4);
        }
        SECTION("Non-sorted input") {
            Dataset d(10, { {0,4}, {0,3}, {0,5} });
            REQUIRE(d.median_runtime_of_all_runs() == 4);
        }
        SECTION("Non-zero error codes replaced by timeout") {
            Dataset d(10, { {0,2}, {1,4} });
            REQUIRE(d.median_runtime_of_all_runs() == 6);
        }
        SECTION("Values over the timeout are trimmed") {
            Dataset d(10, { {0,2}, {0,200} });
            REQUIRE(d.median_runtime_of_all_runs() == 6);
        }
    }

    SECTION("Corner cases")
    {
        SECTION("No samples lead to a zero result") {
            Dataset d(10, {});
            REQUIRE(d.median_runtime_of_all_runs() == 0);
        }

        SECTION("Even samples, calculate mean of the middle two") {
            Dataset d(10, { {0,1}, {0,3}, {0,5}, {0,10} });
            REQUIRE(d.median_runtime_of_all_runs() == 4);
        }

        SECTION("Even samples, round up the mean value") {
            Dataset d(10, { {0,3}, {0,6} });
            REQUIRE(d.median_runtime_of_all_runs() == 5);
        }
    }
}

TEST_CASE("Dataset::mad_runtime_of_all_runs")
{
    SECTION("Typical usage")
    {
        SECTION("Easy case") {
            Dataset d(10, { {0,3}, {0,4}, {0,5}, {0,6}, {0,7} });
            // median of these samples is 5
            // absolute differences are 0, 1, 1, 2, 2
            // median of absolute differences is 1 (= the middle value)
            REQUIRE(d.mad_runtime_of_all_runs() == 1);
        }
        SECTION("Overly large runtime is trimmed") {
            Dataset d(10, { {0,2}, {0,20} });
            // median of samples is 6
            // abs. diff. are 4, 4, their median is 4
            REQUIRE(d.mad_runtime_of_all_runs() == 4);
        }
    }

    SECTION("Corner cases")
    {
        SECTION("No samples lead to a zero result") {
            Dataset d(10, {});
            REQUIRE(d.mad_runtime_of_all_runs() == 0);
        }
        SECTION("Even samples, calculate mean of the middle two") {
            Dataset d(10, { {0,1}, {0,3}, {0,5}, {0,10} });
            // median of these values is 4
            // abs. differnces are 1, 1, 3, 6
            // middle values are 1, 3, their mean is 2
            REQUIRE(d.mad_runtime_of_all_runs() == 2);
        }
        SECTION("Even samples, round up the mean of absolute differences") {
            Dataset d(10, { {0,1}, {0,4}, {0,4}, {0,10} });
            // median of these values is 4
            // abs. differnces are 0, 0, 3, 6
            // middle values are 0, 3
            // their mean is 1.5, rounded up is 2
            REQUIRE(d.mad_runtime_of_all_runs() == 2);
        }
    }
}



TEST_CASE("Dataset::median_runtime_of_successful_runs")
{
    SECTION("Typical usage")
    {
        SECTION("Sorted input") {
            Dataset d(10, { {0,3}, {0,4}, {0,5} });
            REQUIRE(d.median_runtime_of_successful_runs() == 4);
        }
        SECTION("Non-sorted input") {
            Dataset d(10, { {0,4}, {0,3}, {0,5} });
            REQUIRE(d.median_runtime_of_successful_runs() == 4);
        }
        SECTION("Non-zero error codes are skipped") {
            Dataset d(10, { {0,6}, {1,8} });
            REQUIRE(d.median_runtime_of_successful_runs() == 6);
        }
        SECTION("Values over the timeout are trimmed") {
            Dataset d(10, { {0,2}, {0,200} });
            REQUIRE(d.median_runtime_of_successful_runs() == 6);
        }
    }

    SECTION("Corner cases")
    {
        SECTION("No samples lead to a zero result") {
            Dataset d(10, {});
            REQUIRE(d.median_runtime_of_successful_runs() == 0);
        }
        SECTION("No successful sample leads to zero") {
            Dataset d(10, { {1,1}, {2,2} });
            REQUIRE(d.median_runtime_of_successful_runs() == 0);
        }
        SECTION("Even successful samples, calculate mean of the middle two") {
            Dataset d(10, { {0,1}, {0,3}, {1,5} });
            REQUIRE(d.median_runtime_of_successful_runs() == 2);
        }
    }
}



TEST_CASE("Dataset::mad_runtime_of_all_successful_runs")
{
    SECTION("Typical usage")
    {
        SECTION("Non-sorted input with non-successful values") {
            Dataset d(10, { {1,6}, {2,7}, {0,3}, {0,4}, {0,5} });
            // median of succ. samples is 4
            // absolute differences are 0, 1, 1
            // median of absolute differences is 1
            REQUIRE(d.mad_runtime_of_all_successful_runs() == 1);
        }
        SECTION("Non-zero exit codes are removed") {
            Dataset d(10, { {0,3}, {1,4}, {0,5}, {3,6}, {4,7} });
            REQUIRE(d.mad_runtime_of_all_successful_runs() == 1);
        }
        SECTION("Overly large runtime is trimmed") {
            Dataset d(10, { {0,2}, {0,20}, {0,6} });
            // trimmed values are 2, 6, 10, median is 6
            // their abs. diffs. are 0, 4, 4
            REQUIRE(d.mad_runtime_of_all_successful_runs() == 4);
        }
    }

    SECTION("Corner cases")
    {
        SECTION("No samples lead to a zero result") {
            Dataset d(10, {});
            REQUIRE(d.mad_runtime_of_all_successful_runs() == 0);
        }
        SECTION("No successful sample leads to zero") {
            Dataset d(10, { {1,1}, {2,2} });
            REQUIRE(d.mad_runtime_of_all_successful_runs() == 0);
        }
        SECTION("Even successful samples, round up the MAD") {
            Dataset d(10, { {2,1}, {0,4}, {1,4}, {0,9} });
            // median of these values is 7
            // abs. differnces are 2, 3
            // their mean is 2.5, rounded up is 3
            REQUIRE(d.mad_runtime_of_all_successful_runs() == 3);
        }
    }
}

TEST_CASE("Dataset::number_of_all_successful_runs")
{
    SECTION("Typical usage")
    {
        SECTION("Non-zero exit codes are removed 1") {
            Dataset d(10, { {0,3}, {1,4}, {0,5}, {3,6}, {4,7} });
            REQUIRE(d.number_of_all_successful_runs() == 2);
        }
        SECTION("Overly large runtime is trimmed") {
            Dataset d(10, { {0,2}, {0,20}, {0,6} });
            REQUIRE(d.number_of_all_successful_runs() == 2);
        }
        SECTION("Non-zero exit codes are removed 2") {
            Dataset d(10, { {2,1}, {0,4}, {1,4}, {0,9} });
            REQUIRE(d.number_of_all_successful_runs() == 2);
        }
    }

    SECTION("Corner cases")
    {
        SECTION("No samples lead to a zero result") {
            Dataset d(10, {});
            REQUIRE(d.number_of_all_successful_runs() == 0);
        }
        SECTION("No successful sample leads to zero") {
            Dataset d(10, { {1,1}, {2,2} });
            REQUIRE(d.number_of_all_successful_runs() == 0);
        }

    }
}
