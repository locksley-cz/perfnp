cmake_minimum_required(VERSION 3.2)
project(perfnp LANGUAGES CXX VERSION 0.0.1)

# Load coverage-related stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/misc/cmake")
if(PERFNP_ENABLE_COVERAGE)
    set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
    find_package(codecov)
    list(APPEND LCOV_REMOVE_PATTERNS "/usr/*")
endif()

# Disable GNU extensions
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11) #Pavla
set(CMAKE_CXX_EXTENSIONS OFF)



# List sources
set(PERFNP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PERFNP_LIB_DIR ${PERFNP_DIR}/src/perfnp)
set(PERFNP_CLI_DIR ${PERFNP_DIR}/cli)
set(PERFNP_TEST_DIR ${PERFNP_DIR}/tests)

set(PERFNP_HEADER_FILES
    ${PERFNP_LIB_DIR}/combin.hpp
    ${PERFNP_LIB_DIR}/config.hpp
    ${PERFNP_LIB_DIR}/dataset.hpp
    ${PERFNP_LIB_DIR}/exec.hpp
    ${PERFNP_LIB_DIR}/logger.hpp
    ${PERFNP_LIB_DIR}/option.hpp
    ${PERFNP_LIB_DIR}/scheduler.hpp
    ${PERFNP_LIB_DIR}/tools.hpp
)

set(PERFNP_LIB_FILES
    ${PERFNP_LIB_DIR}/combin.cpp
    ${PERFNP_LIB_DIR}/config.cpp
    ${PERFNP_LIB_DIR}/dataset.cpp
    ${PERFNP_LIB_DIR}/exec.cpp
    ${PERFNP_LIB_DIR}/logger.cpp
)

set(PERFNP_TEST_FILES
    ${PERFNP_TEST_DIR}/combin_test.cpp
    ${PERFNP_TEST_DIR}/config_test.cpp
    ${PERFNP_TEST_DIR}/dataset_test.cpp
    ${PERFNP_TEST_DIR}/exec_test.cpp
    ${PERFNP_TEST_DIR}/tools_test.cpp
)

# Use all single-header libraries
include_directories(SYSTEM ${PERFNP_DIR}/lib)

# Use folders to group files when generating project files
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("Headers" FILES ${PERFNP_HEADER_FILES})
source_group("Library" FILES ${PERFNP_LIB_DIR})
source_group("CLI" FILES ${PERFNP_CLI_DIR})
source_group("Tests" FILES ${PERFNP_TEST_DIR})

# Target: The Library
add_library(libperfnp ${PERFNP_LIB_FILES})
target_include_directories(libperfnp PUBLIC
    $<BUILD_INTERFACE:${PERFNP_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
# Effectively set C++11 (at least)
target_compile_features(libperfnp
  INTERFACE
    cxx_alignas
    cxx_alignof
    cxx_attributes
    cxx_auto_type
    cxx_constexpr
    cxx_defaulted_functions
    cxx_deleted_functions
    cxx_final
    cxx_lambdas
    cxx_noexcept
    cxx_override
    cxx_range_for
    cxx_rvalue_references
    cxx_static_assert
    cxx_strong_enums
    cxx_trailing_return_types
    cxx_unicode_literals
    cxx_user_literals
    cxx_variadic_macros
)

# Target: Unit tests
add_executable(tests
    ${PERFNP_TEST_DIR}/catch_main.cpp
    ${PERFNP_TEST_FILES}
)

# Adding CATCH_CONFIG_FAST_COMPILE speeds up compilation
target_compile_definitions(tests PRIVATE
    $<$<PLATFORM_ID:Windows>:CATCH_CONFIG_WINDOWS_CRTDBG>
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # STL iterators are checked on Linux
    target_compile_definitions(tests PRIVATE
        $<$<PLATFORM_ID:Linux>:_HAS_ITERATOR_DEBUGGING>
    )
endif()

# Target: Executable files
add_executable(perfnp ${PERFNP_CLI_DIR}/main.cpp)
set(PERFNP_BINARIES
    perfnp
)

foreach(target tests ${PERFNP_BINARIES})
    target_link_libraries(${target} libperfnp)

    # Enable coverage reporting
    if (PERFNP_ENABLE_COVERAGE)
        add_coverage(${target})
    endif()

    # Compiler-specific flags
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(${target} PUBLIC
        -Wall -Wextra $<$<CXX_COMPILER_ID:Clang>:-Wmissing-noreturn>
    )
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${target} PUBLIC /W4 /wd4800 /wd4200 /wd4245 /w44265 /w44061 /w44062)
    endif()
endforeach()

if(PERFNP_ENABLE_COVERAGE)
    coverage_evaluate()
endif()

### Enable make test and ctest
set(CATCH_FLAGS -r compact -\# ~[.])
include(CTest)

# We compile one large binary, but use Catch's -# flag to group tests by their implementation file
# and register multiple ctest tests (for more granularity and parallelization opportunities)
foreach(test_file ${PERFNP_TEST_FILES})
    get_filename_component(no_ext ${test_file} NAME_WE)
    if( no_ext MATCHES "_test")
        add_test(NAME ${no_ext} COMMAND tests -r compact -\# ~[.] [\#${no_ext}])
        set_tests_properties(${no_ext} PROPERTIES FAIL_REGULAR_EXPRESSION "No tests ran.")
        set_tests_properties(${no_ext} PROPERTIES TIMEOUT 500)
    endif()
endforeach()
