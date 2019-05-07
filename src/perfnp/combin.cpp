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
std::vector<CommandLine> combine(
	const std::vector<std::vector<std::string>>& arr,
	const std::string cmd_local)
{
	std::vector<CommandLine> output; // return value
	const size_t n = arr.size(); // number of arrays

    if (n == 0) {
        return std::vector<CommandLine>();
    }

	// keeps track of next element in each of the n arrays
	std::vector<size_t> indices(n, 0);

	while (1) {

		// print current combination
		std::vector<std::string> vector_string_temp;
		for (int i = 0; i < n; i++) {
			// std::cout << arr[i][indices[i]] << " ";
			vector_string_temp.push_back(arr[i][indices[i]]);
		}
		// std::cout << std::endl;

		CommandLine cmdline_local(cmd_local, vector_string_temp);
		output.push_back(cmdline_local);

		// find the rightmost array that has more elements left
		// after the current element in that array
		size_t next = n - 1;
		while (next >= 0 && (indices[next] + 1 >= arr[next].size())) {
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

} // anonymous namespace

std::vector<CommandLine> perfnp::combine_command_lines(const Config& config)
{
    auto arguments = config.arguments();
	auto parameters = config.parameters();

	// create and fill vector of vectors string which will be combine
	std::vector<std::vector<std::string>> strings_to_combine;
	for (const auto argument : arguments.values()) {

		bool find_par_arg = false;
		for (const auto param : parameters) {
			if (argument == "$" + param.name()) {
				find_par_arg = true;
				strings_to_combine.push_back(param.values());
				break;
			}
		}

		if (find_par_arg == false) {
			strings_to_combine.push_back({argument});
		}
	}

	// std::cout << "strings to make all combinations:" << std::endl;
	// for (int i = 0; i < strings_to_combine.size(); i++) {
	// 	for (int j = 0; j < strings_to_combine.at(i).size(); j++) {
	// 		std::cout << strings_to_combine.at(i).at(j) << " ";
	// 	}
	// 	std::cout << std::endl;
	// }

    auto command = config.command();
	// std::cout << "Making all combinations: " << std::endl;
 	// std::cout << command << std::endl;
	return combine(strings_to_combine, command);
}
