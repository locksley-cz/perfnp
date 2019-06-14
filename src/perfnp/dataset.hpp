// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_DATASET_H_
#define PERFNP_DATASET_H_

#include "perfnp/exec.hpp"

#include <string>
#include <vector>


namespace perfnp {

/*!
 * Dataset contains all data about all runs.
 */
class Dataset {
    //! Timeout which was used during the execution
    unsigned m_timeout;

    //! Results of all runs
    std::vector<ExecResult> m_results;

public:
    //! Initialize all fields by the given values
    Dataset(unsigned timeout, std::vector<ExecResult> results)
    : m_timeout(timeout)
    , m_results(std::move(results))
    {}

    /*!
     * Median runtime from all runs.
     *
     * The runtime-of an unsuccessful run
     * is (by definition) the timeout.
     *
     * @return the median or 0 if no run has happened
     */
     unsigned median_runtime_of_all_runs() const;

    /*!
     * Median absolute deviation runtime from all runs.
     *
     * The runtime-of an unsuccessful run
     * is (by definition) the timeout.
     *
     * @return the mad or 0 if no run has happened
     */
    unsigned mad_runtime_of_all_runs() const;

    /*!
     * Median runtime from all successful runs.
     *
     * @return the median or 0 if no successful run has happened
     */
    unsigned median_runtime_of_successful_runs() const;

    /*!
     * Median absolute deviation runtime from all successful runs.
     *
     * @return the mad or 0 if no successful run has happened
     */
    unsigned mad_runtime_of_all_successful_runs() const;
    //number of successful runs
    unsigned number_of_all_successful_runs() const;
}; // Dataset
} // perfnp
#endif // PERFNP_DATASET_H_
