/*!
 * @brief SamplesProcessor test
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
 * @file samples_processor_test.cpp
 */

#include <vector>
#include <algorithm>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "telemetry/samples_processor.hpp"
#include "telemetry/value_rounder.hpp"
#include "agent-framework/module/enum/common.hpp"

using namespace agent_framework::model;
using namespace telemetry;

namespace {

template<typename SamplesProcessor>
void verify_processor(SamplesProcessor& processor,
                      typename SamplesProcessor::TimePoint::duration sensing_interval,
                      const std::vector<json::Json>& expected_values,
                      const std::vector<json::Json>& sample_values) {
    auto now = SamplesProcessor::Clock::now();
    int i{0};
    for (const auto& value: sample_values) {
        json::Json calculated = processor.add_and_process_samples(value, now);
        if (expected_values[i].is_number()) {
            EXPECT_DOUBLE_EQ(expected_values[i].get<double>(), calculated.get<double>()) << " i=" << i;
        }
        else {
            EXPECT_EQ(expected_values[i].dump(), calculated.dump()) << " i=" << i;
        }
        i++;
        now += sensing_interval;
    }
}

void verify_processor_with_precision(SamplesProcessor<>& processor,
                      const ValueRounder<>& round,
                      SamplesProcessor<>::TimePoint::duration sensing_interval,
                      const std::vector<json::Json>& expected_values,
                      const std::vector<json::Json>& sample_values) {
    auto now = SamplesProcessor<>::Clock::now();
    int i{0};
    for (const auto& value: sample_values) {
        auto calculated = round(processor.add_and_process_samples(value, now));
        if (expected_values[i].is_number()) {
            EXPECT_DOUBLE_EQ(expected_values[i].get<double>(), calculated.get<double>()) << "i=" << i;
        }
        else {
            EXPECT_EQ(expected_values[i].dump(), calculated.dump()) << " i=" << i;
        }
        i++;
        now += sensing_interval;
    }
}

template<class Samples,
        typename std::enable_if<
            std::is_floating_point<typename Samples::value_type::ValueType>::value,
            int>::type = 0>
json::Json simple_average_over_interval(const Samples& samples) {
    using Sample = typename Samples::value_type;
    if (samples.empty()) {
        return json::Json();
    }
    auto sum = std::accumulate(samples.cbegin(), samples.cend(), 0.0,
                               [](double d, const Sample& s) { return d + s.m_value;});
    return sum / double(samples.size());
}

template<class Samples,
        typename std::enable_if<
            std::is_same<typename Samples::value_type::ValueType, json::Json>::value,
            int>::type = 0>
json::Json simple_average_over_interval(const Samples& samples) {
    using Sample = typename Samples::value_type;
    if (samples.empty()) {
        return json::Json();
    }
    auto count_numbers = std::count_if(samples.cbegin(), samples.cend(),
                                       [](const Sample& s) {return s.m_value.is_number();});
    auto sum = std::accumulate(samples.cbegin(), samples.cend(), 0.0,
                               [](double d, const Sample& s) {
                                   return (s.m_value.is_number())
                                          ? (d + s.m_value.template get<double>())
                                          : d;
                               });
    return sum / double(count_numbers);
}

}

TEST(SamplesProcessorTest, ClearOnNullProcessorAddAndProcessSamples) {
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::AverageOverInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    SamplesProcessor<> processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    auto calculated = processor.add_and_process_samples(json::Json());
    EXPECT_TRUE(calculated.is_null());
    calculated = processor.add_and_process_samples(5.0);
    EXPECT_DOUBLE_EQ(5.0, calculated.get<double>());
}

TEST(SamplesProcessorTest, NoClearOnNullProcessorAddAndProcessSamples) {
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::AverageOverInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    using SamplesProcessorNoClearOnNull = SamplesProcessor<Samples<>, false>;
    SamplesProcessorNoClearOnNull processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    auto calculated = processor.add_and_process_samples(json::Json());
    EXPECT_TRUE(calculated.is_null());
    calculated = processor.add_and_process_samples(5.0);
    EXPECT_DOUBLE_EQ(5.0, calculated.get<double>());
}

TEST(SamplesProcessorTest, AverageOverIntervalCalculation) {
    std::vector<json::Json> sample_values =   {nullptr, 2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 3.0, 4.0, nullptr, 4.0, 5.0};
    std::vector<json::Json> expected_values = {nullptr, 2.0, 2.5, 3.5, 4.5, 4.875, 4.8, 4.5, 4.2142857142857144, 4.2857142857142856, 4.1428571428571432, nullptr, 4.0, 4.5};
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::AverageOverInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    SamplesProcessor<> processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    verify_processor(processor, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, AverageOverIntervalCalculationWith0dot5Precision) {
    std::vector<json::Json> sample_values =   {nullptr, 2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 3.0, 4.0, nullptr, 4.0, 5.0};
    std::vector<json::Json> expected_values = {nullptr, 2.0, 2.5, 3.5, 4.5, 5.0, 5.0, 4.5, 4.0, 4.5, 4.0, nullptr, 4.0, 4.5};
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::AverageOverInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    auto calculation_precision = 0.5;
    SamplesProcessor<> processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    ValueRounder<> round(calculation_precision);
    verify_processor_with_precision(processor, round, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, AverageOverIntervalCalculationWith1dot0Precision) {
    std::vector<json::Json> sample_values =   {nullptr, 2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 3.0, 4.0, nullptr, 4.0, 5.0};
    std::vector<json::Json> expected_values = {nullptr, 2.0, 3.0, 4.0, 5.0, 5.0, 5.0, 5.0, 4.0, 4.0, 4.0, nullptr, 4.0, 5.0};
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::AverageOverInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    auto calculation_precision = 1.0;
    SamplesProcessor<> processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    ValueRounder<> round(calculation_precision);
    verify_processor_with_precision(processor, round, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, AverageOverIntervalCalculationWith5dot0Precision) {
    std::vector<json::Json> sample_values =   {nullptr, 2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 3.0, 4.0, nullptr, 4.0, 5.0};
    std::vector<json::Json> expected_values = {nullptr, 0.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, nullptr, 5.0, 5.0};
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::AverageOverInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    auto calculation_precision = 5.0;
    SamplesProcessor<> processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    ValueRounder<> round(calculation_precision);
    verify_processor_with_precision(processor, round, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, MinimumDuringIntervalIntervalCalculation) {
    std::vector<json::Json> sample_values =   {2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 1.0, 0.0, 3.0, -1.0};
    std::vector<json::Json> expected_values = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 1.0, 0.0, 0.0, -1.0};
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::MinimumDuringInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    SamplesProcessor<> processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    verify_processor(processor, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, MaximumDuringIntervalIntervalCalculation) {
    std::vector<json::Json> sample_values =   {2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 9.0, 4.0};
    std::vector<json::Json> expected_values = {2.0, 3.0, 6.0, 7.0, 7.0, 7.0, 7.0, 7.0, 9.0, 9.0};
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::MaximumDuringInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    SamplesProcessor<> processor(calculation_interval, sensing_interval, select_algorithm<typename SamplesProcessor<>::Samples>(calculation_algorithm));
    verify_processor(processor, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, SimpleAverageOverIntervalCalculation) {
    std::vector<json::Json> sample_values =   {nullptr, 2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 3.0, 4.0, nullptr, 4.0, 5.0};
    std::vector<json::Json> expected_values = {nullptr, 2.0, 2.5, 3.6666666666666665, 4.5, 4.6, 4.5, 4.1428571428571432, 4.0, 4.125, 4.25, nullptr, 4.0, 4.5};
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    SamplesProcessor<> processor(calculation_interval, sensing_interval, simple_average_over_interval<typename SamplesProcessor<>::Samples>);
    verify_processor(processor, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, SimpleAverageOverIntervalCalculationNullDoesNotClear) {
    std::vector<json::Json> sample_values =   {2.0, 3.0, 6.0,                7.0, nullptr, 5.0, 4.0, 2.0,                3.0, 3.0,   4.0,  nullptr, 4.0, 5.0};
    std::vector<json::Json> expected_values = {2.0, 2.5, 3.6666666666666665, 4.5, 4.5,     4.6, 4.5, 4.1428571428571432, 4.2857142857142856, 4.2857142857142856, 4.0, 3.5, 3.5714285714285716, 3.5714285714285716};
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    using SamplesProcessorNoClearOnNull = SamplesProcessor<Samples<>, false>;
    SamplesProcessorNoClearOnNull processor(calculation_interval, sensing_interval, simple_average_over_interval<typename SamplesProcessorNoClearOnNull::Samples>);
    verify_processor(processor, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, LatestValueAlgorithm) {
    std::vector<json::Json> sample_values =   {nullptr, 2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 3.0, 4.0, nullptr, 4.0, 5.0};
    std::vector<json::Json> expected_values = {nullptr, 2.0, 3.0, 6.0, 7.0, 5.0, 4.0, 2.0, 3.0, 3.0, 4.0, nullptr, 4.0, 5.0};
    using Samples = typename SamplesProcessor<>::Samples;
    auto calculation_algorithm = [](const Samples & samples) { return samples.empty() ? json::Json() : json::Json(samples.back().m_value);};
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    SamplesProcessor<> processor(calculation_interval, sensing_interval, calculation_algorithm);
    verify_processor(processor, sensing_interval, expected_values, sample_values);
}

TEST(SamplesProcessorTest, MaximumDuringIntervalCalculationJsonSamples) {
    std::vector<json::Json> sample_values =   {"aa", "bb", nullptr};
    std::vector<json::Json> expected_values = {"aa", "bb", "bb"};
    using JsonSamples = Samples<json::Json>;
    using JsonSamplesProcessor = SamplesProcessor<JsonSamples, false>;
    auto calculation_algorithm = agent_framework::model::enums::MetricAlgorithm::MaximumDuringInterval;
    auto calculation_interval = std::chrono::seconds(7);
    auto sensing_interval = std::chrono::seconds(1);
    JsonSamplesProcessor processor(calculation_interval, sensing_interval, select_algorithm<typename JsonSamplesProcessor::Samples>(calculation_algorithm));
    verify_processor(processor, sensing_interval, expected_values, sample_values);
}
