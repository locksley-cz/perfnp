// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/exec.hpp"

#include <cassert>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <limits>

#if defined(__linux__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <memory>

#elif defined(_WIN32)
#include <limits>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <windows.h>
#include <IntSafe.h>

#else
#error "Unsupported platform."
#endif

using namespace perfnp;
using namespace std::chrono;

ExecBin::ExecBin(
    const std::string& binary,
    const std::vector<std::string>& args,
    unsigned timeout)
: m_binary(binary)
, m_args(args)
, m_timeout(timeout)
{
    if (binary.empty()) {
        throw std::runtime_error("Name of the executable must not be empty.");
    }
} // ExecBin::ExecBin



#if defined(__linux__) || defined(__APPLE__)
ExecResult ExecBin::execute() const
{
    auto start_time = steady_clock::now();

    pid_t child_proc_id = fork();
    if (child_proc_id == -1) {
        throw std::runtime_error(
            "fork() failed: errno "
            + std::to_string(errno) );

    } else if (child_proc_id == 0) {
        // Child process

        // 1) Prepare arguments for execvp
        const char *file = m_binary.c_str();
        std::unique_ptr<char*[]> argv(new char*[m_args.size() + 2]);

        argv[0] = const_cast<char*>(m_binary.c_str());
        for (size_t i = 0; i < m_args.size(); ++i) {
            argv[i + 1] = const_cast<char*>(m_args[i].c_str());
        }
        argv[m_args.size() + 1] = NULL;

        // 2) Execute the process in the child
        alarm(m_timeout); // setup the time-out
        int retval = execvp(file, argv.get());
        if (retval == -1) {
            throw std::runtime_error( "execvp("
                + m_binary + ", ...) failed: errno="
                + std::to_string(errno) );
        } else {
            throw std::runtime_error(
                "execvp(...) must return -1, but it didn't");
        }

    } else {
        // Parent process

        // 1) Wait for the child process
        int status;
        if (waitpid(child_proc_id, &status, 0) == -1) {
            throw std::runtime_error(
                "waitpid(...) returned -1: errno="
                    + std::to_string(errno));
        }

        // 2) Measure elapsed time
        auto elapsed_in_ms = duration_cast<milliseconds>
                (steady_clock::now() - start_time).count();
        auto elapsed_in_s = elapsed_in_ms / 1000;
        if (elapsed_in_ms % 1000 > 0) {
            elapsed_in_s += 1;
        }

        // 3) Child process exited normally
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            return ExecResult(exit_code, elapsed_in_s);

        // 4) Child exited because of a signal
        } else if (WIFSIGNALED(status)) {
            // Signal is the SIGALRM = timeout occurred
            if (WTERMSIG(status) == SIGALRM) {
                return ExecResult(-1, elapsed_in_s);
            }

            throw std::runtime_error("unknown signal: "
                + std::to_string(WTERMSIG(status)));
        } else {
            throw std::runtime_error("cause of death not determined");
        }
    }
} // ExecBin::execute
#endif



#if defined(_WIN32)
namespace {

/**
 * This routine appends the given argument to a command line such
 * that CommandLineToArgvW will return the argument string unchanged.
 * Arguments in a command line should be separated by spaces;
 * this function does not add these spaces.
 *
 * Source:
 * https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
 *
 * \param[in] Argument supplies the argument to encode.
 * \param[out] CommandLine supplies the command line to which
 *             we append the encoded argument string.
 * \param[in] Force supplies an indication of whether we should quote
 *            the argument even if it does not contain any characters
 *            that would ordinarily require quoting.
 */
void ArgvQuote(const std::wstring& Argument,
    std::wstring& CommandLine, bool Force)
{
    // Unless we're told otherwise, don't quote unless we actually
    // need to do so --- hopefully avoid problems if programs won't
    // parse quotes properly
    if (Force == false
            && Argument.empty() == false
            && Argument.find_first_of(L" \t\n\v\"") == Argument.npos) {
        CommandLine.append(Argument);
    } else {
        CommandLine.push_back(L'"');

        for (auto It = Argument.begin(); ; ++It) {
            unsigned NumberBackslashes = 0;

            while (It != Argument.end() && *It == L'\\') {
                ++It;
                ++NumberBackslashes;
            }

            if (It == Argument.end()) {
                // Escape all backslashes, but let the terminating
                // double quotation mark we add below be interpreted
                // as a metacharacter.
                CommandLine.append(NumberBackslashes * 2, L'\\');
                break;
            } else if (*It == L'"') {
                // Escape all backslashes and the following
                // double quotation mark.
                CommandLine.append(NumberBackslashes * 2 + 1, L'\\');
                CommandLine.push_back(*It);
            } else {
                // Backslashes aren't special here.
                CommandLine.append(NumberBackslashes, L'\\');
                CommandLine.push_back(*It);
            }
        }

        CommandLine.push_back(L'"');
    }
} // ArgvQuote



/*!
 * Convert a UTF-8 encoded string to `std::wstring`.
 */
std::wstring from_utf8(const std::string& utf8_string) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(utf8_string);
    return wide;
}



/**
 * Calls a clean-up method in the destructor.
 *
 * Allocate this object to ensure that any subsequent
 * `return` calls the `CloseHandle` on the given handle.
 */
struct HandleGuard {

    HANDLE m_handle;

    HandleGuard(HANDLE handle)
        : m_handle(handle) {}

    ~HandleGuard() {
        if (CloseHandle(m_handle) == 0) {
            std::cerr << "CloseHandle has failed: ERROR ";
            std::cerr << GetLastError() << std::endl;
            abort(); // Fail fast
        }
    }
}; // HandleGuard

} // empty namespace



ExecResult ExecBin::execute() const
{
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFOW);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::wstring command_line;
    ArgvQuote(from_utf8(m_binary), command_line, false);
    for (const auto& arg : m_args) {
        command_line += L" ";
        ArgvQuote(from_utf8(arg), command_line, false);
    }

    // Do not use/modify/... command_line after this line:
    LPWSTR command_line_buf = const_cast<wchar_t*>(command_line.c_str());

    // Start the child process.
    auto start_time = std::chrono::steady_clock::now();
    if (!CreateProcessW(
        NULL,           // No module name (use command line)
        command_line_buf, // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW, // Suppress stdout and stderr
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) {
        switch (GetLastError()) {
            case ERROR_FILE_NOT_FOUND:
                throw std::runtime_error("Command '"
                    + m_binary + "' not found");
            default:
                throw std::runtime_error(
                    "CreateProcess failed: ERROR "
                    + std::to_string(GetLastError()));
        }
    }

    // Close all handles on any path
    HandleGuard hProcessGuard(pi.hProcess);
    HandleGuard hThreadGuard(pi.hThread);

    // In order to terminate child jobs, we must group them to a job.
    HANDLE job_handle = CreateJobObjectA(NULL, NULL);
    if (GetLastError() != 0) {
        throw std::runtime_error(
            "CreateJobObjectA failed: ERROR "
            + std::to_string(GetLastError()));
    }
    HandleGuard job_handle_guard(job_handle);
    // Set memory limit for the job
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_limits;
    job_limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_JOB_MEMORY;
    job_limits.JobMemoryLimit = 1024L * 1024L * 1024L * 4L - 1;
    if (!SetInformationJobObject(job_handle, JobObjectExtendedLimitInformation,
                &job_limits, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION) )) {
        throw std::runtime_error(
            "SetInformationJobObject failed: ERROR "
            + std::to_string(GetLastError()));
    }
    // Assign the process to the job
    if (!AssignProcessToJobObject(job_handle, pi.hProcess)) {
        throw std::runtime_error(
            "AssignProcessToJobObject failed: ERROR "
            + std::to_string(GetLastError()));
    }

    // Start measuring the elapsed time
    DWORD timeout = m_timeout; // in s
    timeout = timeout * 1000; // in ms
    if (timeout == 0) { // no time-out
        timeout = INFINITE;
    }

    // Wait until child process exits.
    DWORD wait_for_child_retval = WaitForSingleObject(pi.hProcess, timeout);
    auto end_time = std::chrono::steady_clock::now();

    // Calculate the runtime
    auto elapsed_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    auto elapsed_in_s = elapsed_in_ms / 1000;
    if (elapsed_in_ms % 1000 > 0) {
        elapsed_in_s += 1;
    }

    switch (wait_for_child_retval) {

        case WAIT_FAILED:
            throw std::runtime_error(
                "WaitForSingleObject failed: ERROR "
                + std::to_string(GetLastError()) );

        case WAIT_TIMEOUT:
            if (!TerminateJobObject(job_handle, 0)) {
                throw std::runtime_error(
                    "TerminateJobObject failed: ERROR "
                    + std::to_string(GetLastError()) );
            }
            return ExecResult(128, elapsed_in_s);

        case WAIT_OBJECT_0:
            DWORD error_code;
            if (GetExitCodeProcess(pi.hProcess, &error_code)) {
                return ExecResult(error_code, elapsed_in_s);
            } else {
                throw std::runtime_error(
                    "GetExitCodeProcess failed: ERROR "
                    + std::to_string(GetLastError()) );
            }

        default:
            throw std::runtime_error(
                "WaitForSingleObject returned an unexpected value: "
                + std::to_string(wait_for_child_retval) + ", ERROR: "
                + std::to_string(GetLastError()) );
    }
} // ExecBin::execute

#endif // defined(_WIN32)
