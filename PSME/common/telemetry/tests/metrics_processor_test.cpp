/*!
 * @brief MetricsProcessor tests
 *
 * Each object derived from this class represents metric (value).
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
 * @file metrics_processor_test.cpp
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <thread>


#include <iostream>

#include "telemetry_reader_mock.hpp"
#include "telemetry/metric_processor.hpp"
#include "telemetry/metric_definition_builder.hpp"
#include "agent-framework/module/enum/common.hpp"

using namespace agent_framework::model;
using namespace telemetry;
using ::testing::_;

namespace ipmi {

class MockIpmiController : public ipmi::IpmiController {
public:
    MockIpmiController() : IpmiController(nullptr) {
        EXPECT_CALL(*this, send(_, _, _)).Times(0);
    }

    MOCK_METHOD2(send, void(const Request&, Response&));
    MOCK_METHOD3(send, void(const Request&, const BridgeInfo&, Response&));
};

}

namespace {

constexpr char INTERVAL_STR_100MS[] = "PT0.1s";
constexpr char INTERVAL_STR_10S[] = "PT10s";

TelemetryReader::TimePoint get_now() {
    return TelemetryReader::TimePoint::clock::now();
}

}

TEST(MetricsProcessorTest, Constructor) {
    ipmi::MockIpmiController ipmi_controller{};
    MetricDefinition TEST_METRIC_DEFINITION =
        MetricDefinitionBuilder("/TestMetricDefinition")
            .set_metric_type(enums::MetricType::Numeric)
            .set_sensing_interval(INTERVAL_STR_10S)
            .build();
    std::vector<json::Json> sample_values =   {2.0, 2.0};
    TelemetryReader::PtrVector readers{
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values),
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values)
    };
    const auto num_readers = readers.size();
    MetricsProcessor metric_processor{ipmi_controller, std::move(readers)};
    EXPECT_EQ(num_readers, metric_processor.get_reader_ptrs().size());
    auto now = get_now();
    const auto update_time = metric_processor.get_earliest_update_time();
    EXPECT_LE(now, update_time);
    EXPECT_GE(now + std::chrono::seconds(5), update_time);
}

TEST(MetricsProcessorTest, GetEarliestUpdateTime) {
    ipmi::MockIpmiController ipmi_controller{};
    MetricDefinition TEST_METRIC_DEFINITION =
        MetricDefinitionBuilder("/TestMetricDefinition")
            .set_metric_type(enums::MetricType::Numeric)
            .set_sensing_interval(INTERVAL_STR_10S)
            .build();
    std::vector<json::Json> sample_values =   {2.0, 2.0};
    TelemetryReader::PtrVector readers{
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values),
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values)
    };
    const auto num_readers = readers.size();
    MetricsProcessor metric_processor{ipmi_controller, std::move(readers)};
    EXPECT_EQ(num_readers, metric_processor.get_reader_ptrs().size());
    auto now = get_now();
    auto update_time = metric_processor.get_earliest_update_time();
    EXPECT_LE(now, update_time);
    metric_processor.read_all_metrics();
    update_time = metric_processor.get_earliest_update_time();
    EXPECT_LE(now + std::chrono::seconds(10), update_time);
    EXPECT_GE(now + std::chrono::seconds(20), update_time);
}

TEST(MetricsProcessorTest, ReadAllMetricsReaderValueDoesNotChange) {
    ipmi::MockIpmiController ipmi_controller{};
    MetricDefinition TEST_METRIC_DEFINITION =
        MetricDefinitionBuilder("/TestMetricDefinition")
            .set_metric_type(enums::MetricType::Numeric)
            .set_sensing_interval(INTERVAL_STR_100MS)
            .build();
    std::vector<json::Json> sample_values =   {2.0, 2.0};
    std::vector<json::Json> expected_values = {2.0, 2.0};
    TelemetryReader::PtrVector readers{
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values),
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values)
    };
    const auto num_readers = readers.size();
    MetricsProcessor metric_processor{ipmi_controller, std::move(readers)};
    int i = 0;
    // subsequent reads should not change value
    for (const auto& expected : expected_values) {
        auto begin = metric_processor.get_earliest_update_time();
        const auto changed_readers = metric_processor.read_all_metrics();
        const auto sleep_time = metric_processor.get_earliest_update_time() - begin;
        if (0 == i) {
            EXPECT_EQ(num_readers, changed_readers.size());
        }
        else {
            EXPECT_EQ(0, changed_readers.size());
        }
        for (const auto& reader_ptr : metric_processor.get_reader_ptrs()) {
            const auto& reader = *reader_ptr;
            EXPECT_FALSE(reader.is_to_be_read());
            EXPECT_TRUE(reader.fills_metric());
            EXPECT_FALSE(reader.fills_health());
            EXPECT_EQ(ResourceInstance{ResourceInstance::Component::System}, reader.get_resource_key());
            if (expected.is_number()) {
                EXPECT_LE(begin, *reader.get_time_to_update());
                EXPECT_NEAR(expected.get<double>(), reader.get_value().get<double>(), 0.001);
            }
            else {
                EXPECT_TRUE(reader.get_value().is_null());
            }
        }
        ++i;
        std::this_thread::sleep_for(sleep_time);
    }
}

TEST(MetricsProcessorTest, ReadAllMetricsReaderValueChanges) {
    ipmi::MockIpmiController ipmi_controller{};
    MetricDefinition TEST_METRIC_DEFINITION =
        MetricDefinitionBuilder("/TestMetricDefinition")
            .set_metric_type(enums::MetricType::Numeric)
            .set_sensing_interval(INTERVAL_STR_100MS)
            .build();
    std::vector<json::Json> sample_values =   {2.0, 3.0, nullptr, 4.0};
    std::vector<json::Json> expected_values = {2.0, 3.0, nullptr, 4.0};
    TelemetryReader::PtrVector readers{
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values),
        std::make_shared<TelemetryReaderValuesFromArray>(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, sample_values)
    };
    const auto num_readers = readers.size();
    MetricsProcessor metric_processor{ipmi_controller, std::move(readers)};
    for (const auto& expected : expected_values) {
        auto begin = metric_processor.get_earliest_update_time();
        const auto changed_readers = metric_processor.read_all_metrics();
        const auto sleep_time = metric_processor.get_earliest_update_time() - begin;
        EXPECT_EQ(num_readers, changed_readers.size());
        for (const auto& reader_ptr : changed_readers) {
            const auto& reader = *reader_ptr;
            EXPECT_FALSE(reader.is_to_be_read());
            EXPECT_TRUE(reader.fills_metric());
            EXPECT_FALSE(reader.fills_health());
            EXPECT_EQ(ResourceInstance{ResourceInstance::Component::System}, reader.get_resource_key());
            if (expected.is_number()) {
                EXPECT_LE(begin, *reader.get_time_to_update());
                EXPECT_NEAR(expected.get<double>(), reader.get_value().get<double>(), 0.001);
            }
            else {
                EXPECT_TRUE(reader.get_value().is_null());
            }
        }
        std::this_thread::sleep_for(sleep_time);
    }
}