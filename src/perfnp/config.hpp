// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_CONFIG_H_
#define PERFNP_CONFIG_H_

#include "option.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace perfnp {

/*!
 * Parameter is a variable with several values it can take.
 */
class Parameter {

    //! Name of the variable
    std::string m_name;

    //! Values the variable can take
    std::vector<std::string> m_values;

public:
    //! Default arguments leaves all fields empty
    Parameter() = default;

    //! Initialize all fields by the given values
    explicit Parameter(std::string name, std::vector<std::string> values)
    : m_name(name)
    , m_values(values)
    {}

    //! Name of the variable
    const std::string& name() const {
        return m_name;
    }

    //! Values the variable can take
    const std::vector<std::string>& values() const {
        return m_values;
    }

    //! Equality operator compares all fields
    bool operator==(const Parameter& rhs) const {
        return m_name == rhs.m_name
            && m_values == rhs.m_values;
    }
};



/*!
 * List of command-line arguments
 */
class Arguments {

    //! List of command-line arguments
    std::vector<std::string> m_args;

public:
    //! Default arguments leaves all fields empty
    Arguments() = default;

    //! Initialize all fields by the given values
    explicit Arguments(std::vector<std::string> values)
    : m_args(values)
    {}

    //! List of command-line arguments
    const std::vector<std::string>& values() const {
        return m_args;
    }

    //! Equality operator compares all fields
    bool operator== (const Arguments& rhs) const {
        return m_args == rhs.m_args;
    }
};



/*!
 * Configuration file contains all the program input
 */
class Config {

    //! Parsed JSON
    nlohmann::json m_json;

public:

    //! Create a config file given a JSON input
    Config(const nlohmann::json& json)
    : m_json(json)
    {}

    //! Create a config file given a JSON input
    Config(nlohmann::json&& json)
    : m_json(json)
    {}

    //! Time-limit to execute the binary, in seconds
    unsigned timeout() const;

    //! Absolute or relative path to the executed binary
    std::string command() const;

    //! List of arguments given to the binary
    Arguments arguments() const;

    //! List of all parameters and their values
    std::vector<Parameter> parameters() const;

    //! File name for the CSV job log
    Optional<std::string> logging_job_csv_file() const;

    //! Print the JSON to a string
    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Config& cfg);
}; // Config

//! Print the config JSON to a stream
std::ostream& operator<<(std::ostream& os, const Config& cfg);


} // perfnp
#endif // PERFNP_CONFIG_H_
