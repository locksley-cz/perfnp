// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/cmd_line.hpp"
#include <locale> 
#include <codecvt>
#include <sstream>
#include <iostream>
namespace {

    //! If this string was printed in Bash unqoted, would it contain special characters?
    bool utf8_string_needs_no_escaping_for_bash(const std::string& str)
    {
        for (char ch : str) {
            if (ch >= 'a' && ch <= 'z') continue;
            if (ch >= 'A' && ch <= 'Z') continue;
            if (ch >= '0' && ch <= '9') continue;
            if (ch == '_') continue;
            if (ch == '-') continue;
            return false;
        }
        return true;
    } // utf8_string_needs_no_escaping_for_bash



    //! Escapes an UTF-8 encoded string so that Bash sees is as 1 argument
    void escape_utf8_string_for_bash(std::ostream& os, const std::string& str)
    {
        if (utf8_string_needs_no_escaping_for_bash(str)) {
            os << str;
            return;
        }

        os << '\'';
        for (char ch : str) {
            if (ch == '\'') {
                os << '\''; // stop the string
                os << '\\' << '\''; // escaped apostrophe
                os << '\''; // resume the string
            } else {
                os << ch;
            }
        }
        os << '\'';
    } // escape_utf8_string_for_bash



    /*!
     * Escape utf16 string for CommandLineToArgvW.
     *
     * This routine appends the given argument to a command line such
     * that CommandLineToArgvW will return the argument string unchanged.
     * Arguments in a command line should be separated by spaces;
     * this function does not add these spaces.
     *
     * Source:
     * https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
     *
     * \param[out] os stream to which we append the encoded argument string
     * \param[in] str supplies the argument to encode
     * \param[in] force supplies an indication of whether we should quote
     *            the argument even if it does not contain any characters
     *            that would ordinarily require quoting.
     */
    void escape_utf16_string_for_command_prompt(std::wostream& os,
        const std::wstring& str, bool force)
    {
        // Unless we're told otherwise, don't quote unless we actually
        // need to do so --- hopefully avoid problems if programs won't
        // parse quotes properly
        if (!force && !str.empty() && str.find_first_of(L" \t\n\v\"") == str.npos) {
            os << str;
            return;
        }

        os << L'"';
        for (auto It = str.begin(); ; ++It) {
            unsigned NumberBackslashes = 0;

            while (It != str.end() && *It == L'\\') {
                ++It;
                ++NumberBackslashes;
            }

            if (It == str.end()) {
                // Escape all backslashes, but let the terminating
                // double quotation mark we add below be interpreted
                // as a metacharacter.
                for (unsigned i = 0; i < NumberBackslashes * 2; i++) {
                    os << L'\\';
                }
                break;
            } else if (*It == L'"') {
                // Escape all backslashes and the following
                // double quotation mark.
                for (unsigned i = 0; i <= NumberBackslashes * 2; i++) {
                    os << L'\\';
                }
                os << *It;
            } else {
                // Backslashes aren't special here.
                for (unsigned i = 0; i < NumberBackslashes * 2; i++) {
                    os << L'\\';
                }
                os << *It;
            }
        }
        os << L'"';
    } // ArgvQuote



    //! Converts a UTF-8 encoded string to a UTF-16 string.
    std::wstring utf8_to_utf16(const std::string& utf8_string) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(utf8_string);
    } // utf8_to_utf16



    //! Converts a UTF-8 encoded string to a UTF-16 string.
    std::string utf16_to_utf8(const std::wstring& utf16_string) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(utf16_string);
    } // utf16_to_utf8

} // anonymous ns



std::string perfnp::CmdWithArgs::escape_for_native_shell() const
{
#if defined(_WIN32)
    return utf16_to_utf8(escape_for_cmd_exe());
#else
    // Hopefully, we are on Linux.
    // TODO: Print a warning to std::cerr if not.
    return escape_for_bash();
#endif
} // escape_for_native_shell



std::string perfnp::CmdWithArgs::escape_for_bash() const
{
    std::stringstream stream;
    stream << *this;
    return stream.str();
} // escape_for_bash



std::wstring perfnp::CmdWithArgs::escape_for_cmd_exe() const
{
    std::wostringstream stream;
    stream << *this;
    return stream.str();
} // escape_for_cmd_exe



std::ostream& perfnp::operator<<(std::ostream& os, const CmdWithArgs& cwa)
{
    escape_utf8_string_for_bash(os, cwa.command());

    for (const auto& arg : cwa.arguments()) {
        os << ' ';
        escape_utf8_string_for_bash(os, arg);
    }

    return os;
} // operator<<(ostream, CmdWithArgs)



std::wostream& perfnp::operator<<(std::wostream& os, const CmdWithArgs& cwa)
{
    escape_utf16_string_for_command_prompt(os, utf8_to_utf16(cwa.command()), false);
    for (const auto& arg : cwa.arguments()) {
        os << L' ';
        escape_utf16_string_for_command_prompt(os, utf8_to_utf16(arg), false);
    }

    return os;
} // operator<<(wostream, CmdWithArgs)
