// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_CORE_H_
#define PERFNP_CORE_H_

#include <perfnp/tools.hpp>

#include <string>
#include <vector>

namespace perfnp {

/**
 * Exit status of a process
 */
class ExecResult {

    /*!
     * Exit code (aka error-level) of the process.
     */
    int m_exit_code;

    /*!
     * Runtime of the program, in seconds.
     *
     * The value is rounded up, therefore
     * 0 should never appear here.
     */
    unsigned m_runtime;

public:
    //! Initialize all values and check their validity.
    ExecResult(int exit_code, unsigned runtime)
    : m_exit_code(exit_code)
    , m_runtime(runtime)
    {
        if (runtime == 0) {
            throw std::runtime_error("Runtime was 0,"
            " which is not a result of ceil(positive value).");
        }
    }

    /*!
     * Exit code (aka error-level) of the process.
     *
     * The value 0 means that the process exited
     * successfully within the timeout. Any other
     * value is an error value (without
     * a standardized meaning).
     */
    int exit_code() const {
        return m_exit_code;
    }

    /*!
     * Runtime of the program, in seconds.
     *
     * Value is rounded up, therefore
     * 0 should never be returned.
     */
    unsigned runtime() const {
        return m_runtime;
    }
}; // ExecResult



/**
 * Execute a binary
 */
class ExecBin {

    /** Name of the executed file */
    std::string m_binary;

    /** Arguments to the executed file */
    std::vector<std::string> m_args;

    /**
     * Execution timeout in seconds
     *
     * Zero value means no timeout.
     */
    unsigned m_timeout;

public:

    /**
     * Default constructor merely initializes all fields.
     */
    ExecBin(
        const std::string& binary,
        const std::vector<std::string>& args
            = std::vector<std::string>(),
        unsigned timeout = 0);

    /** Name of the executed file */
    const std::string& binary() const
    {
        return m_binary;
    }

    /** Execute the binary */
    ExecResult execute() const;

}; // ExecBin
} // perfnp
#endif // PERFNP_CORE_H_
