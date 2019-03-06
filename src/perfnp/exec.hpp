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
 * Execute a binary
 */
class ExecBin {

    /** Name of the executed file */
    std::string m_binary;

    /** Arguments to the executed file */
    std::vector<std::string> m_args;

public:

    /**
     * Default constructor merely initializes all fields.
     */
    ExecBin(
        const std::string& binary,
        const std::vector<std::string>& args = std::vector<std::string>());

    /** Name of the executed file */
    const std::string& binary() const
    {
        return m_binary;
    }

}; // ExecBin
} // perfnp
#endif // PERFNP_CORE_H_
