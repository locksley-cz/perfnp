#include "perfnp/dataset.hpp"
#include "perfnp/exec.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace perfnp;
using namespace std;

namespace {

// Function for calculating median
unsigned findMedian(std::vector<unsigned> a)
{
    const unsigned n = a.size();
    if (n == 0) {
        return 0;
    }

    std::sort(a.begin(), a.end());

    if (n % 2 != 0) {
        return a[(n+1)/2 - 1];
    }

    auto sum = a[(n-1)/2] + a[n/2];
    if (sum % 2 != 0) {
        sum += 1;
    }

    return sum / 2;
}

unsigned medianAbsoluteDeviation(std::vector<unsigned> a)
{
    const unsigned n = a.size();
    std::vector<unsigned> b(n);

    const unsigned med = findMedian(a);

    for (unsigned i = 0; i < n; i++) {
        if (a[i] < med) {
            b[i] = med - a[i];
        } else {
            b[i] = a[i] - med;
        }
    }

    return findMedian(b);
}

std::vector<unsigned> calculate_time_all(
    const std::vector<ExecResult>& results,
    unsigned timeout)
{
    std::vector<unsigned> time_all;

    if (results.empty()) {
        time_all.push_back(0);

    } else {
        for (size_t i=0; i < results.size();i++) {
            if (results.at(i).exit_code() == 0 && results.at(i).runtime() < timeout)
            {
                time_all.push_back(results.at(i).runtime());
            }
            else
            {
                time_all.push_back(timeout);
            }
        }
    }
    return time_all;
}

std::vector<unsigned> calculate_time_success(
    const std::vector<ExecResult>& results,
    unsigned timeout)
{
    std::vector<unsigned> time_s;
    if (results.empty()) {
        time_s.push_back(0);
    } else {
        int status = 0;
        for (size_t i=0; i < results.size();i++)
        {
            if (results.at(i).exit_code() == 0) {
                if (results.at(i).runtime() <= timeout) {
                    time_s.push_back(results.at(i).runtime());
                }
                else if(results.at(i).runtime() >= timeout)
                {
                    time_s.push_back(timeout);
                }
            } else {
                status++;
            }
        }
        if (status == results.size()) {
            time_s.push_back(0);
        }
    }
    return time_s;
}

unsigned calculate_number_of_successful_runs(
    const std::vector<ExecResult>& results,
    unsigned timeout)
{
    int n_success = 0;
        for (size_t i=0; i < results.size();i++)
        {
            if ((results.at(i).exit_code() == 0)&&(results.at(i).runtime() <= timeout)) {
                n_success++;
            }
        }
    return n_success;
}

} // anonymous names

unsigned perfnp::Dataset::median_runtime_of_all_runs() const
{
    auto runtimes = calculate_time_all(m_results, m_timeout);
    return findMedian(runtimes);
}

unsigned perfnp::Dataset::mad_runtime_of_all_runs() const
{
    auto runtimes = calculate_time_all(m_results, m_timeout);
    return medianAbsoluteDeviation(runtimes);
}

unsigned perfnp::Dataset::median_runtime_of_successful_runs() const
{
    auto runtimes = calculate_time_success(m_results, m_timeout);
    return findMedian(runtimes);
}

unsigned perfnp::Dataset::mad_runtime_of_all_successful_runs() const
{
    auto runtimes = calculate_time_success(m_results, m_timeout);
    return medianAbsoluteDeviation(runtimes);
}
unsigned perfnp::Dataset::number_of_all_successful_runs() const
{
    auto number_success = calculate_number_of_successful_runs(m_results, m_timeout);
    return number_success;
}
