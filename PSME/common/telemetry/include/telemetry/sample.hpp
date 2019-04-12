/*!
 * @brief Sample
 *
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file sample.hpp
 */

#pragma once

#include <algorithm>
#include <vector>

namespace telemetry {

/*! Sample value with timepoint */
template<typename T, typename C>
struct Sample {
    using ValueType = T;
    using Clock = C;
    using TimePoint = typename Clock::time_point;

    /*! Default Constructor */
    Sample() : Sample(ValueType(), TimePoint()) {}

    /*! Constructor
     * @param v sample value
     * @param t sample timepoint
     */
    Sample(ValueType v, TimePoint t) : m_value(v), m_timepoint(t) {}

    ValueType m_value;
    TimePoint m_timepoint;
};

/*! Compares Samples by values */
template<typename Sample>
bool sample_by_value_comp(const Sample& a, const Sample& b) {
    return a.m_value < b.m_value;
}

/*! Compares Samples by timepoint */
template<typename Sample>
bool sample_by_time_comp(const Sample& a, const Sample& b) {
    return a.m_timepoint < b.m_timepoint;
}

/*! Sample container */
template<typename T = double,
         typename C = std::chrono::steady_clock>
using Samples = std::vector<Sample<T, C>>;

/*! Removes "old" samples before given timepoint.
 *  We assume samples are ordered by timepoint
 *  (first sample has lower timepoint than next ones).
 */
template<typename Samples>
void remove_samples_before_timepoint(Samples& samples, typename Samples::value_type::TimePoint time_point) {
    using Sample = typename Samples::value_type;
    samples.erase(std::begin(samples),
                  std::find_if(std::begin(samples), std::end(samples), [time_point](const Sample& s) {
                      return time_point <= s.m_timepoint;
                  }));
}

}
