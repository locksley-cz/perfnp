// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_CONFIG_H_
#define PERFNP_CONFIG_H_

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
    Parameter(std::string name, std::vector<std::string> values)
    : m_name(name)
    , m_values(values)
    {}

    //! Equality operator compares all fields
    bool operator==(const Parameter& rhs) const {
        return m_name == rhs.m_name
            && m_values == rhs.m_values;
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

    //! Time-limit to execute the binary, in seconds
    unsigned timeout() const;

    //! Absolute or relative path to the executed binary
    std::string command() const;

    //! List of arguments given to the binary
    std::vector<std::string> arguments() const;

    //! List of all parameters and their values
    std::vector<Parameter> parameters() const;

}; // Config
} // perfnp
#endif // PERFNP_CONFIG_H_
