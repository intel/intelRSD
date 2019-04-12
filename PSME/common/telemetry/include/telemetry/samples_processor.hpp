/*!
 * @brief SamplesProcessor
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
 * @file samples_processor.hpp
 */

#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <vector>
#include <string>
#include "sample.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace telemetry {

/*! Samples calculation algorithm */
template<typename Samples>
using CalculationAlgorithm = std::function<json::Json(const Samples&)>;

template<typename Samples>
json::Json average_over_interval(const Samples& samples) {
    using namespace std::chrono;
    if (0 == samples.size()) {
        return json::Json();
    }
    else if (1 == samples.size()) {
        return json::Json(samples.back().m_value);
    }
    auto it = samples.cbegin();
    auto prev_sample = *it++;
    double area = 0.0;
    for (; it != samples.cend(); ++it) {
        area += (double(prev_sample.m_value) + double(it->m_value)) / 2.0
                 * duration_cast<duration<double>>((it->m_timepoint - prev_sample.m_timepoint)).count();
        prev_sample = *it;
    }
    return json::Json(area / duration_cast<duration<double>>(
            (prev_sample.m_timepoint - samples.front().m_timepoint)).count());
}

template<typename Samples>
json::Json minimum_during_interval(const Samples& samples) {
    auto it = std::min_element(samples.cbegin(), samples.cend(),
                               sample_by_value_comp<typename Samples::value_type>);
    if (it == samples.cend()) {
        return json::Json();
    }
    return json::Json(it->m_value);
}

template<typename Samples>
json::Json maximum_during_interval(const Samples& samples) {
    auto it = std::max_element(samples.cbegin(), samples.cend(),
                               sample_by_value_comp<typename Samples::value_type>);
    if (it == samples.cend()) {
        return json::Json();
    }
    return json::Json(it->m_value);
}

template<typename T>
struct SampleTraits {
    static constexpr bool accepts_null() noexcept { return false; }
    static typename T::ValueType null() noexcept { return typename T::ValueType(); }
};

template<>
struct SampleTraits<json::Json> {
    static constexpr bool accepts_null() noexcept { return true; }
    static json::Json null() noexcept { return json::Json(); }
};

/*!
 * Processes samples according to given calculation parameters
 */
template<typename T = Samples<>,
         bool CLEAR_ON_NULL = true>
class SamplesProcessor {
public:
    using Samples = T;
    using Sample = typename T::value_type;
    using STraits = SampleTraits<Sample>;
    using ValueType = typename Sample::ValueType;
    using Clock = typename Sample::Clock;
    using TimePoint = typename Sample::TimePoint;

    /*!
     * Constructor
     * @param calculation_interval Samples calculation interval
     * @param sensing_interval Samples sensing interval
     * @param algorithm Calculation algorithm
     */
    SamplesProcessor(typename TimePoint::duration calculation_interval,
                     typename TimePoint::duration sensing_interval,
                     CalculationAlgorithm<Samples> algorithm)
        : m_calculation_interval{calculation_interval}, m_algorithm{algorithm} {

        if (sensing_interval > TimePoint::duration::zero()) {
            m_samples.reserve(typename Samples::size_type(calculation_interval/sensing_interval + 1));
        }
    }

    /*!
     * Appends new sample to sample collection and calculates new value over samples.
     * If necessary trims sample collection to fit calculation interval window.
     *
     * @param value New sample value
     * @param now Sample's timepoint
     * @return Json value with value calculated over samples
     */
    json::Json add_and_process_samples(const json::Json& value, TimePoint now = Clock::now()) {
        const auto is_null = value.is_null();
        if (CLEAR_ON_NULL && is_null) {
            m_samples.clear();
        }
        else {
            remove_samples_before_timepoint<Samples>(m_samples, now - m_calculation_interval);
            if (!is_null) {
                m_samples.emplace_back(value.get<ValueType>(), now);
            }
            else {
                if (STraits::accepts_null()) {
                    m_samples.emplace_back(STraits::null(), now);
                }
                else {
                    log_debug("samples_processor", "skipping null");
                }
            }
        }
        return m_algorithm(m_samples);
    }

private:
    typename TimePoint::duration m_calculation_interval;
    CalculationAlgorithm<Samples> m_algorithm;
    Samples m_samples{};
};

/*! Calculation algorithm selector */
template<typename T>
CalculationAlgorithm<T> select_algorithm(agent_framework::model::enums::MetricAlgorithm metric_algorithm) {
    switch (metric_algorithm) {
    case agent_framework::model::enums::MetricAlgorithm::AverageOverInterval:
        return average_over_interval<T>;
    case agent_framework::model::enums::MetricAlgorithm::MaximumDuringInterval:
        return maximum_during_interval<T>;
    case agent_framework::model::enums::MetricAlgorithm::MinimumDuringInterval:
        return minimum_during_interval<T>;
    default:
        break;
    };
    return [](const T& samples) {
        return samples.empty() ? json::Json() : json::Json(samples.back().m_value);
    };
}

}
