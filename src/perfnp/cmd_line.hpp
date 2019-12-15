// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_CMD_LINE_H_
#define PERFNP_CMD_LINE_H_

#include <perfnp/tools.hpp>

#include <string>
#include <vector>

namespace perfnp {

/*!
 * Command to be executed with arguments
 */
class CmdWithArgs {

    //! Index of this run (see combin.hpp)
    unsigned m_run_index;

    //! Command to be executed
    std::string m_command;

    //! List of arguments for the command
    std::vector<std::string> m_arguments;

public:
    CmdWithArgs(
        unsigned job_index,
        std::string command,
        std::vector<std::string> arguments)
    : m_run_index(job_index)
    , m_command(std::move(command))
    , m_arguments(std::move(arguments))
    {}

    //! Index of this run (see combin.hpp)
    unsigned job_index() const
    {
        return m_run_index;
    }

    //! Command to be executed
    const std::string& command() const
    {
        return m_command;
    }

    //! List of arguments for the command
    const std::vector<std::string>& arguments() const
    {
        return m_arguments;
    }

    /*!
     * Returns a native-shell-friendly string in UTF-8.
     *
     * Method is platform-dependent. On Windows, we create
     * a Command Prompt (cmd.exe) friendly string, on other
     * platforms, we assume Linux and prepare a Bash-friendly
     * string.
     */
    std::string escape_for_native_shell() const;

    //! Escape command and arguments for Linux Bash.
    std::string escape_for_bash() const;

    //! Escape command and arguments for Windows Command Prompt
    std::wstring escape_for_cmd_exe() const;

    bool operator==(const CmdWithArgs& rhs) const {
        return m_run_index == rhs.m_run_index
            && m_command == rhs.m_command
            && m_arguments == rhs.m_arguments;
    }

    bool operator!=(const CmdWithArgs& rhs) const {
        return ! ((*this) == rhs);
    }

}; // CmdWithArgs

//! Serialize the command with arguments to a stream, escape for Bash
std::ostream& operator<<(std::ostream& os, const perfnp::CmdWithArgs& cwa);

//! Serialize the command with arguments to an UTF-16 stream, escape for Command Prompt (cmd.exe)
std::wostream& operator<<(std::wostream& os, const perfnp::CmdWithArgs& cwa);

} // perfnp

#endif // PERFNP_CMD_LINE_H_
