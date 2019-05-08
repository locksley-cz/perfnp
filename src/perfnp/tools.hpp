// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_TOOLS_H_
#define PERFNP_TOOLS_H_

#include <stdexcept>
#include <string>
#include <typeinfo>
#include <type_traits>



// ======================================================= //
// C++11 and C++17 compatible "if constexpr (...) { ... }" //
// ======================================================= //

#if __cplusplus > 201703L
    // C++17
    #define PERFNP_IF_CONSTEXPR(x) if constexpr (x)
#else
    #if defined(_MSC_VER)
        // pre-C++17 MSVC
        #define PERFNP_IF_CONSTEXPR(x) __pragma(warning(push)) __pragma(warning(disable:4127)) if (x) __pragma(warning(pop))
    #else
        // pre-C++17 GCC/clang
        #define PERFNP_IF_CONSTEXPR(x) if (x)
    #endif
#endif



// ================================================ //
// Convert any type (including objects) into string //
// ================================================ //

#if defined(__GNUC__)
#include <cxxabi.h>
#include <cstdlib>

#elif defined(_WIN32)
#include <sstream>

#else
#error "Unsupported platform"
#endif

namespace perfnp {
namespace tools {

    template<class type>
    std::string type_to_string()
    {
#if defined(__GNUC__)
        int status;
        char* name = abi::__cxa_demangle(
            typeid(type).name(),
            nullptr, 0, &status
        );

        if (status == 0) {
            if (name != nullptr) {
                std::string type_name(name);
                free(name);
                return type_name;
            } else {
                throw std::runtime_error(std::string(
                    "unknown error, abi::__cxa_demangle did not return the name"
                ));
            }

        } else if (status == -1) {
            throw std::bad_alloc();

        } else {
            throw std::runtime_error(std::string(
                "unknown error, abi::__cxa_demangle returned "
            ) + std::to_string(status) );
        }

#elif defined(_WIN32)
        std::stringstream type_name_stream;
        type_name_stream << typeid(type).name();
        auto type_name_str = type_name_stream.str();

        // Try removing "class " from the beginning
        if (type_name_str.size() > 6 &&
            type_name_str.substr(0, 6) == "class ") {
            return type_name_str.substr(6);
        } else {
            return type_name_str;
        }
#else
#error "Unsupported platform"
#endif
    } // type_to_string

} // tools
} // perfnp
#endif // PERFNP_TOOLS_H_
