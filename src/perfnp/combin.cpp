// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "perfnp/combin.hpp"
#include "perfnp/config.hpp"

#include <nlohmann/json.hpp>
#include <iostream>

using namespace perfnp;

namespace { // hide the combine method

// function to print combinations that contain
// one element from each of the given arrays
std::vector<std::vector<std::string>> combine(
	const std::vector<std::vector<std::string>>& input)
{
	std::vector<std::vector<std::string>> output; // return value
	const size_t n = input.size(); // number of arrays

    if (n == 0) {
        return {};
    }

	// keeps track of next element in each of the n arrays
	std::vector<size_t> indices(n, 0);

	while (1) {

		// print current combination
		std::vector<std::string> vector_string_temp;
		for (int i = 0; i < n; i++) {
			// std::cout << arr[i][indices[i]] << " ";
			vector_string_temp.push_back(input[i][indices[i]]);
		}

		output.push_back(vector_string_temp);

		// find the rightmost array that has more elements left
		// after the current element in that array
		size_t next = n - 1;
		while (next >= 0 && (indices[next] + 1 >= input[next].size())) {
			if (next > 0) {
				next--; // proceed to the next array
			} else {
				// no such array is found so no more combinations left
				return output;
			}
		}

		// if found move to next element in that array
		indices[next]++;

		// for all arrays to the right of this
		// array current index again points to
		// first element
		for (size_t i = next + 1; i < n; i++) {
			indices[i] = 0;
		}
	}
} // combine

void replace(std::string& haystack,
       const std::string& needle,
       const std::string& replacement)
{
    std::size_t pos;
    while ((pos = haystack.find(needle)) != std::string::npos) {
        haystack = haystack.replace(
            pos, needle.length(),
            replacement);

    }
} // replace

} // anonymous namespace



std::vector<CmdWithArgs> perfnp::combine_command_lines(const Config& config)
{
    auto command = config.command();
    auto arguments = config.arguments();
	auto parameters = config.parameters();

    if (parameters.empty()) {
        return { CmdWithArgs(0, command, arguments.values()) };
    }

    std::vector<CmdWithArgs> out;

    std::vector<std::vector<std::string>> param_values;
    for (const auto parameter : parameters) {
        param_values.push_back(parameter.values());
    }

    std::vector<std::vector<std::string>> combinations = combine(param_values);
    for (const auto& combination : combinations) {
        assert(combination.size() == parameters.size());

        std::vector<std::string> substituted;
        for (std::string argument : arguments.values()) {
            for (size_t i = 0; i < parameters.size(); ++i) {
                replace(argument, "%" + parameters[i].name() + "%", combination[i]);
            }
            substituted.emplace_back(std::move(argument));
        }
        out.emplace_back(CmdWithArgs(out.size(), command, std::move(substituted)));
    }
    return out;
}
