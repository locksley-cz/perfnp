// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/config.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cassert>
#include <string>

using namespace perfnp;
using namespace std;


unsigned Config::timeout() const
{
    if (m_json.find("timeout") == m_json.end()) {
        throw std::runtime_error("Configuration JSON"
            " does not have the \"timeout\" field.");
    }

    if (!m_json.at("timeout").is_number()) {
        throw std::runtime_error("Configuration JSON's"
            " \"timeout\" field is not a number.");
    }

    return m_json.at("timeout").get<unsigned>();
}



std::string Config::command() const {
    if (m_json.find("command") == m_json.end()) {
        throw std::runtime_error("Configuration JSON"
            " does not have the \"command\" field.");
    }

    if (!m_json.at("command").is_string()) {
        throw std::runtime_error("Configuration JSON's"
            " \"command\" field is not a string.");
    }

    return m_json.at("command").get<std::string>();
}



std::vector<std::string> Config::arguments() const {
    if (m_json.find("arguments") == m_json.end()) {
        throw std::runtime_error("Configuration JSON"
            " does not have the \"arguments\" field.");
    }

    const auto& j_arguments = m_json.at("arguments");

    if (!j_arguments.is_array()) {
        throw std::runtime_error("Configuration JSON's"
            " \"arguments\" field is not an array.");
    }

    std::vector<std::string> v_arguments;

    for (auto index = 0; index < j_arguments.size(); index++) {
        const auto& j_item = j_arguments.at(index);

        if (!j_item.is_string()) {
        throw std::runtime_error(std::string("Configuration JSON's"
            " \"arguments\" array must contain strings, but '")
            + j_item.dump() + "' was found instead.");
        }
        v_arguments.push_back(j_item.get<std::string>());
    }

    return v_arguments;
}



std::vector<Parameter> Config::parameters() const {
    if (m_json.find("parameters") == m_json.end()) {
        return {};
    }

    if (!m_json.at("parameters").is_object()) {
        throw std::runtime_error("Configuration JSON's"
            " \"parameters\" field is not an object.");
    }

    std::vector<Parameter> v_parameters;
    for (const auto& kv : m_json.at("parameters").items()) {

        if (kv.key().size() == 0) {
            throw std::runtime_error("Parameters in the"
                " configuration JSON must have a non-empty name.");
        }

        // Variable has a single value.
        if (kv.value().is_string()) {
            v_parameters.push_back(Parameter(kv.key(), {kv.value().get<std::string>()}));

        // Variable has multiple values.
        } else if (kv.value().is_array()) {

            std::vector<std::string> s_values;
            for (const auto& j_value : kv.value()) {
                if (!j_value.is_string()) {
                    throw std::runtime_error(std::string(
                        "Parameters in the configuration JSON take"
                        " string values, but variable '") + kv.key()
                        + "' got '" + j_value.dump() + "' instead.");
                }
                s_values.push_back(j_value.get<std::string>());
            }

            if (s_values.empty()) {
                throw std::runtime_error("Parameters in the"
                    " configuration JSON must have at least 1 value.");
            }

            v_parameters.push_back(Parameter(kv.key(), s_values));

        } else {
            throw std::runtime_error(std::string(
                "Parameters in the configuration JSON take values"
                " as a vector of strings, but variable '") + kv.key()
                + "' got '" + kv.value().dump() + "' instead.");
        }
    }

    return v_parameters;
}
