/*!
 * @brief Telemetry reader test
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
 * @file telemetry_reader_test.cpp
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "telemetry_reader_mock.hpp"
#include "telemetry/metric_definition_builder.hpp"
#include "agent-framework/module/enum/common.hpp"

using namespace agent_framework::model;
using namespace telemetry;

namespace {

constexpr char INTERVAL_5S_STR[] = "PT5S";
constexpr char INTERVAL_500MS_STR[] = "PT0.5s";

MetricDefinition TEST_METRIC_DEFINITION =
        MetricDefinitionBuilder("/TestMetricDefinition")
            .set_metric_type(enums::MetricType::Numeric)
            .build();

MetricDefinition TEST_METRIC_DEFINITION_WITH_INTERVAL =
        MetricDefinitionBuilder("/TestMetricDefinitionWithInterval")
            .set_metric_type(enums::MetricType::Numeric)
            .set_sensing_interval(INTERVAL_5S_STR)
            .build();

}

TEST(TelemetryReaderTest, ReaderAfterConstruction) {
    TelemetryReaderValuesFromArray reader(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, {1.0});
    EXPECT_TRUE(reader.fills_metric());
    EXPECT_FALSE(reader.fills_health());
    EXPECT_FALSE(reader.get_time_to_update());
    EXPECT_EQ(agent_framework::model::enums::Health::OK, reader.get_health());
    EXPECT_EQ(ResourceInstance{ResourceInstance::Component::System}, reader.get_resource_key());
    const auto& value = reader.get_value();
    EXPECT_TRUE(value.is_null());
    EXPECT_TRUE(reader.is_to_be_read());
    const auto metric_definition = reader.get_metric_definition();
    EXPECT_EQ(std::chrono::seconds{10}, metric_definition.get_sensing_period().as<std::chrono::seconds>());
}

TEST(TelemetryReaderTest, SetSensingIntervalMetricDefinitionWithIntervalNotDefined) {
    TelemetryReaderValuesFromArray reader(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, {1.0});
    const auto& metric_definition = reader.get_metric_definition();
    EXPECT_FALSE(metric_definition.get_sensing_interval().has_value());
    reader.set_sensing_interval(INTERVAL_500MS_STR);
    EXPECT_TRUE(metric_definition.get_sensing_interval().has_value());
    EXPECT_EQ(std::chrono::milliseconds{500}, metric_definition.get_sensing_period().get_milliseconds());
}

TEST(TelemetryReaderTest, SetSensingIntervalMetricDefinitionWithIntervalDefined) {
    TelemetryReaderValuesFromArray reader(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION_WITH_INTERVAL, {1.0});
    const auto& metric_definition = reader.get_metric_definition();
    EXPECT_TRUE(metric_definition.get_sensing_interval().has_value());
    EXPECT_EQ(INTERVAL_5S_STR, metric_definition.get_sensing_interval().value());
    // note: setting interval has no effect if it's defined in metric definition
    //       this may(should) be changed
    reader.set_sensing_interval(INTERVAL_500MS_STR);
    EXPECT_EQ(INTERVAL_5S_STR, metric_definition.get_sensing_interval().value());
}

TEST(TelemetryReaderTest, SetShoreupPeriodMetricDefinitionWithShoreUpPeriodNotDefined) {
    TelemetryReaderValuesFromArray reader(ResourceInstance{ResourceInstance::Component::System}, TEST_METRIC_DEFINITION, {1.0});
    const auto& metric_definition = reader.get_metric_definition();
    EXPECT_EQ(10.0, metric_definition.get_shoreup_period());
    reader.set_shoreup_period(INTERVAL_500MS_STR);
    EXPECT_EQ(0.5, metric_definition.get_shoreup_period());
}
