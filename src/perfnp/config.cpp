// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
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



Arguments Config::arguments() const {
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

    return Arguments(v_arguments);
}



std::vector<Parameter> Config::parameters() const {

    if (m_json.find("parameters") == m_json.end()) {
        return {};
    }

    if (!m_json.at("parameters").is_array()) {
        throw std::runtime_error("Configuration JSON's"
            " \"parameters\" field is not an array.");
    }

    if (m_json.at("parameters").empty()) {
        throw std::runtime_error("Configuration JSON's"
            " \"parameters\" array is empty.");
    }

    std::vector<Parameter> retval;

    std::vector<Parameter> v_parameters;
    for (const auto& j_param : m_json.at("parameters")) {

        // Name

        if (j_param.find("name") == j_param.end()) {
            throw std::runtime_error("Configuration JSON's"
                " \"parameters\"[] has not an \"name\" field.");
        }

        const auto& j_name = j_param.at("name");

        if (!j_name.is_string()) {
            throw std::runtime_error("Configuration JSON's"
                " \"parameters\"[].\"name\" is not a string.");
        }

        std::string s_name = j_name.get<std::string>();

        if (s_name.empty()) {
            throw std::runtime_error("One parameter's "
                "name in the configuration JSON is empty.");
        }

        if (j_param.find("values") == j_param.end()) {
            throw std::runtime_error("Configuration JSON's"
                " \"parameters\"[] has not the \"values\" field.");
        }

        const auto& j_values = j_param.at("values");

        if (!j_values.is_array()) {
            throw std::runtime_error("Configuration JSON's"
                " \"parameters\"[].\"values\" is not an array.");
        }

        std::vector<std::string> s_values;
        for (const auto& j_value : j_values) {
            if (!j_value.is_string()) {
                throw std::runtime_error(std::string(
                    "Parameters in the configuration JSON take"
                    " string values, but variable '") + s_name
                    + "' got '" + j_value.dump() + "' instead.");
            }
            s_values.push_back(j_value.get<std::string>());
        }

        if (s_values.empty()) {
            throw std::runtime_error("Parameters in the"
                " configuration JSON must have at least 1 value.");
        }

        v_parameters.push_back(Parameter(j_name, s_values));
    }

    return v_parameters;
}



Optional<std::string> Config::logging_job_csv_file() const
{
    auto j_logging = m_json.find("logging");
    if (j_logging == m_json.end()) {
        return Optional<std::string>::empty();
    }
    if (!j_logging->is_object()) {
        throw std::runtime_error("The 'logging' field in the"
                        " configuration json is not an object.");
    }

    auto j_job = j_logging->find("job");
    if (j_job == j_logging->end()) {
        return Optional<std::string>::empty();
    }
    if (!j_job->is_object()) {
        throw std::runtime_error("The 'logging.job' field in the"
                        " configuration json is not an object.");
    }

    auto j_csv = j_job->find("csv");
    if (j_csv == j_job->end()) {
        return Optional<std::string>::empty();
    }

    if (!j_csv->is_string()) {
        throw std::runtime_error("The 'logging.job.csv' field in the"
                        " configuration json is not a string.");
    }

    return Optional<std::string>::make(j_csv->get<std::string>());
}



std::ostream& perfnp::operator<<(std::ostream& os, const Config& cfg)
{
    return os << cfg.m_json;
}



std::string Config::to_string() const
{
    std::stringstream stream;
    stream << *this;
    return stream.str();
}
